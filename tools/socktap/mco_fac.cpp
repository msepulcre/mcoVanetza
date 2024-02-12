#include "mco_fac.hpp"
#include "logs_handler.hpp"
#include <cstring>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vanetza/btp/ports.hpp>
#include <vanetza/asn1/cam.hpp>
#include <vanetza/asn1/packet_visitor.hpp>
#include <vanetza/facilities/cam_functions.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>

using namespace vanetza;
using namespace vanetza::facilities;
using namespace std::chrono;

using namespace Logging;

#define NOT_DEFINED -1;

McoFac::McoFac(PositionProvider& positioning, Runtime& rt) :
    positioning_(positioning), runtime_(rt), mco_interval_(milliseconds(100)), adapt_delta(1) //mco interval es 100 ms
{
    schedule_timer();
}

McoFac::DataConfirm McoFac::mco_data_request(const DataRequest& request, DownPacketPtr packet, PortType PORT)
{
    
    LogsHandler* pLogger = NULL; // Create the object pointer for Logger Class
    pLogger = LogsHandler::getInstance();
    struct timeval te;
    gettimeofday(&te,NULL);
    std::stringstream ss;
    ss << "Transmision de "<< PORT.get() << ","<< 1;
    pLogger->info(ss.str().c_str());

    DataConfirm confirm(DataConfirm::ResultCode::Rejected_Unspecified);

    byte_counter_update(packet->size());

    register_packet(PORT, packet->size(), std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    
    confirm = Application::request(request, std::move(packet), PORT);
    return confirm;
}

void McoFac::register_packet(PortType PORT, float msgSize, int64_t msgTime ){

    McoAppRegister* app_registered;

    for(McoAppRegister&iter : my_list){

        if(iter.PORT_ == PORT){
            
            app_registered = &iter;
            break;

        }

    }

    app_registered->msg_data_list.push_back({msgSize, msgTime});

    std::cout << "El tamaño de la lista de datos de la aplicacion cuyo puerto es " << app_registered->PORT_ << " es " 
    << app_registered->msg_data_list.size() << std::endl;
    

}

void McoFac::register_app(PortType PORT, vanetza::Clock::duration& interval_,  Application& application){ 
            
    my_list.push_back(McoAppRegister(PORT, interval_, application));
    std::cout << "Se ha registrado la aplicacion con el puerto: " << PORT << std::endl;

    std::cout << "El numero de aplicaciones registradas es: " << my_list.size() << std::endl;

}

void McoFac::clean_outdated(){

    const int delated_time = 5000000; //microseconds
    auto current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    for(auto iter_app = my_list.begin() ; iter_app != my_list.end() ; iter_app++ ){

        for(auto iter_data = iter_app->msg_data_list.begin() ; iter_data != iter_app->msg_data_list.end() ;){

            if(current_time - iter_data->msgTime > delated_time){

                iter_data = iter_app->msg_data_list.erase(iter_data);

            }
            else {

                ++iter_data;

            }

        }

    }

}

void McoFac::apps_average_size(){

    for(McoAppRegister& iter_app : my_list){
        
        /* if(iter_app.msg_data_list.size() > 0){ */
            int num_iter_data = 0;
            double data_sum = 0;

            for(auto iter_data : iter_app.msg_data_list ){

                data_sum = data_sum + iter_data.msgSize;
                num_iter_data++;

            }

            if(num_iter_data != 0){
                iter_app.size_average = data_sum / num_iter_data;
            }
        
        /* }else{

            iter_app.size_average = NOT_DEFINED;

        } */
    }

}

void McoFac::apps_average_interval(){

    for(McoAppRegister& iter_app :  my_list){
        
        /* if(iter_app.msg_data_list.size() > 1){ */

            int num_iter_data = 0;
            int64_t data_sum = 0;

            for(auto iter_data = iter_app.msg_data_list.begin() ; iter_data != iter_app.msg_data_list.end(); iter_data++){
            
                auto iter_data_next = iter_data;
                iter_data_next++;

                if(iter_data_next != iter_app.msg_data_list.end()){
            
                    data_sum = data_sum + (iter_data_next->msgTime - iter_data->msgTime);
                    num_iter_data++;

                }

            }   
            if(num_iter_data != 0){
                iter_app.interval_average  = data_sum / num_iter_data;
            }
        /* } else{

            iter_app.interval_average = NOT_DEFINED;

        } */
    }

}

void McoFac::calc_adapt_delta(){

    const double alpha = 0.016;
    const double beta = 0.0012;

    double delta_offset = beta *  (CBR_target - CBR);
    adapt_delta = (1 - alpha) * adapt_delta + delta_offset;

    if(adapt_delta < 0){
        
        adapt_delta = 0;

    }

    LogsHandler* pLogger = NULL; // Create the object pointer for Logger Class
    pLogger = LogsHandler::getInstance();
    struct timeval te;
    gettimeofday(&te,NULL);
    long long current_time=te.tv_sec*1000LL+te.tv_usec/1000;
    std::stringstream ss;
    ss << "Delta," << adapt_delta ;
    pLogger->info(ss.str().c_str());

    std::cout << "adapt_delta: " << adapt_delta << std::endl;
    
}

void McoFac::set_adapt_interval(){

    const double data_speed = 0.75; // byte/us

    if(my_list.size() != 0){
        
        double ACRi = adapt_delta;   

        for(int i = 0; i < 4; i++){ //itera en cada traffic class

            double fraction_time = 0;
            
            for(auto iter_app : my_list){

                if(iter_app.traffic_class_ == i){

                    fraction_time += ((iter_app.size_average / data_speed) / (iter_app.min_interval)); // us / us
                    // fraccion de tiempo que la clase i pide
                }

            }

            if(fraction_time < ACRi){ //si la fraccion de tiempo que se pide es menor que la que se ofrece:

                for(McoAppRegister& iter_app : my_list){

                    if(iter_app.traffic_class_ == i){

                        apps_set_interval(iter_app, iter_app.min_interval);
                        
                    }

                }
                ACRi -= fraction_time;

            } else{ //si es mayor
                double CRi = 0;

                for(McoAppRegister& iter_app : my_list){ //Inicializacion de CRi

                    if((iter_app.traffic_class_ == i) && (iter_app.size_average > 0)){

                        double CREij = ((iter_app.size_average / data_speed) / (iter_app.interval_average));
                        //recursos consumidos por aplicacion j de traffic class i
                        CRi += CREij;
                        //recursos totales consumidos por traffic class i
                    }

                }

                for(McoAppRegister& iter_app : my_list){

                    if(iter_app.traffic_class_ == i){ 

                        double CREij;

                        double ACRij;

                        if(ACRi == 0 ){

                            //ACRij = 0.001; // esto es dejar mas fraccion de tiempo que el maximo permitido por app (0.0005467)

                            apps_set_interval(iter_app, 9999999999); //provisional


                        } else{
                            
                            CREij = ((iter_app.size_average / data_speed) / (iter_app.interval_average)); //us / us

                            ACRij = (CREij / CRi) * ACRi;

                            int64_t Tonij = iter_app.size_average / data_speed; //us

                            int64_t Toffij = Tonij *((1 - ACRij) / ACRij); //us

                            apps_set_interval(iter_app, (Tonij + Toffij));
                        }
                        

                    }

                }

                ACRi = 0;
                
            }

        }

    }
    
}

int McoFac::rand_number(){

    srand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return rand() % 4; 

}

Application& McoFac::search_port(vanetza::btp::port_type PORT){

    for(McoAppRegister& iter : my_list){

        if(iter.application_.port() == PORT){

            Application& application = iter.application_;

            return application;

        }

    }
    Application* application;

    return *application; //no deberia devolver nunca este, en cuyo caso, la aplicacion buscada no existe
}

void McoFac::byte_counter_update(unsigned packet_size){

    const unsigned BTP_header = 4;

    const unsigned GeoNetworking_header = 60;

    const unsigned extra_test = 795; //para subir CBR artificialmente

    //CBR = 10% -> extra_test = 0
    //CBR = 30% -> extra_test = 195
    //CBR = 50% -> extra_test = 395
    //CBR = 70% -> extra_test = 595
    //CBR = 90% -> extra_test = 795

    const unsigned header_size = BTP_header + GeoNetworking_header + extra_test;

    packet_size += header_size;

    byte_counter += packet_size;

}

void McoFac::CBR_update(){

    const double data_speed = (6.0/8.0); // bytes/microseconds (6Mbps)

    double CBR_meassured = (byte_counter / data_speed)/mco_interval_.count(); //microseconds

    CBR = (CBR_meassured + CBR) * 0.5;

    LogsHandler* pLogger = NULL; // Create the object pointer for Logger Class
    pLogger = LogsHandler::getInstance();
    struct timeval te;
    gettimeofday(&te,NULL);
    long long current_time=te.tv_sec*1000LL+te.tv_usec/1000;
    std::stringstream ss;
    ss <<"CBR," << CBR;
    pLogger->info(ss.str().c_str());

    std::cout << "CBR: " << CBR << std::endl;

    byte_counter = 0;

}

void McoFac::set_min_interval(vanetza::btp::port_type PORT, vanetza::Clock::duration interval){

    for(auto& iter_app : my_list){

        if(iter_app.PORT_ == PORT){

            iter_app.min_interval = interval.count();

        }
    }

}

void McoFac::set_apps_number(){

    for(McoAppRegister& iter_app : my_list){

        apps_number[iter_app.traffic_class_]++;
    }

}

void McoFac::set_traffic_class(int traffic_class, vanetza::btp::port_type PORT){

    for(auto& iter_app : my_list){

        if(iter_app.PORT_ == PORT){

            if(traffic_class < 4 && traffic_class >= 0){

                iter_app.traffic_class_ = traffic_class;
    
            } else{

                iter_app.traffic_class_ = 3;

            }

        }

    }

}

void McoFac::apps_set_interval(McoAppRegister &iter_app, int64_t update_interval){

    if(iter_app.interval_.count() > update_interval){ //si el nuevo interval es menor que el antiguo: se para el temporizador
        
        iter_app.application_.set_interval(std::chrono::microseconds(update_interval));

    } else{ //si el nuevo interval es mayor: no se para

        iter_app.interval_ = std::chrono::microseconds(update_interval);

    }

    std::cout << "El intervalo de la aplicacion " << iter_app.PORT_ << " es " << update_interval << std::endl;

    LogsHandler* pLogger = NULL; // Create the object pointer for Logger Class
    pLogger = LogsHandler::getInstance();
    struct timeval te;
    gettimeofday(&te,NULL);
    long long current_time=te.tv_sec*1000LL+te.tv_usec/1000;
    std::stringstream ss;
    ss << "Intervalo de "<< iter_app.PORT_ << "," << update_interval;
    pLogger->info(ss.str().c_str());

    //Podriamos hacer que el temporizador pare si el intervalo nuevo es menor a un 90% del inferior 
}

void McoFac::set_interval(Clock::duration interval)
{
    mco_interval_ = interval;
    runtime_.cancel(this);
    schedule_timer();
}


void McoFac::print_generated_message(bool flag)
{
    print_tx_msg_ = flag;
}

void McoFac::print_received_message(bool flag)
{
    print_rx_msg_ = flag;
}

McoFac::PortType McoFac::port()
{
    return btp::ports::MCO;
}

void McoFac::indicate(const DataIndication& indication, UpPacketPtr packet)
{   
    std::cout << "MCO received a packet" << std::endl;
    Application& application = search_port(indication.destination_port);

    if(indication.destination_port == btp::ports::CAM || indication.destination_port == btp::ports::CAM1 || indication.destination_port == btp::ports::CAM2
     || indication.destination_port == btp::ports::CAM3 || indication.destination_port == btp::ports::CAM4){

        asn1::PacketVisitor<asn1::Cam> visitor;
        std::shared_ptr<const asn1::Cam> cam = boost::apply_visitor(visitor, *packet);
        byte_counter_update(cam->size());
    }

    application.indicate(indication, std::move(packet));

    /* std::cout << "MCO " << (cam ? "decodable" : "broken") << " content" << std::endl; */
    
}

void McoFac::schedule_timer()
{
    runtime_.schedule(mco_interval_, std::bind(&McoFac::on_timer, this, std::placeholders::_1), this);
}

void McoFac::on_timer(Clock::time_point)
{   
    
    schedule_timer();

    clean_outdated();

    apps_average_size();
    
    apps_average_interval();

    CBR_update();

    calc_adapt_delta();

    set_adapt_interval();



    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}