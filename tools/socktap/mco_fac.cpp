#include "mco_fac.hpp"
/* #include <log4cxx/logger.h> */
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

/* using namespace log4cxx; */

McoFac::McoFac(PositionProvider& positioning, Runtime& rt) :
    positioning_(positioning), runtime_(rt), mco_interval_(milliseconds(100)), adapt_delta(1)
{
    schedule_timer();
}

McoFac::DataConfirm McoFac::mco_data_request(const DataRequest& request, DownPacketPtr packet, PortType PORT)
{
    
    DataConfirm confirm(DataConfirm::ResultCode::Rejected_Unspecified);

    byte_counter  += packet->size();

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
            
    my_list.push_back(McoAppRegister(PORT, interval_, rand_traffic_class(), application));
    std::cout << "Se ha registrado la aplicacion con el puerto: " << PORT << std::endl;

    std::cout << "El numero de aplicaciones registradas es: " << my_list.size() << std::endl;

}

void McoFac::clean_outdated(){

    const float delated_time = 5000000; //microseconds
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
        
        int num_iter_data = 0;
        float data_sum = 0;

        for(auto iter_data : iter_app.msg_data_list ){

            data_sum = data_sum + iter_data.msgSize;
            num_iter_data++;

        }

        if(num_iter_data != 0){
            iter_app.size_average = data_sum / num_iter_data;
        }
    }

}

void McoFac::apps_average_interval(){

    for(McoAppRegister& iter_app :  my_list){

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
    }

}

void McoFac::calc_adapt_delta(){


    const double alpha = 0.016;
    const double beta = 0.0012;

    double delta_offset = beta *  (CBR_target - CBR);
    adapt_delta = (1 - alpha) * adapt_delta + delta_offset;
    std::cout << "adapt_delta: " << adapt_delta << std::endl;
    
}

void McoFac::set_adapt_interval(){

    const double data_speed = 0.75; // byte/us

    if(my_list.size() != 0){

        float ACRi = adapt_delta;

        for(int i = 0; (i < 4) && (ACRi != 0) ; i++){ //itera en cada traffic class

            float fraction_time = 0;
            
            for(auto iter_app : my_list){

                if(iter_app.traffic_class_ == i){

                    fraction_time += ((iter_app.size_average / data_speed) / (iter_app.min_interval)); // us / us
                    // fraccion de tiempo que la clase i pide
                }

            }

            if(fraction_time <= ACRi){ //si la fraccion de tiempo que se pide es menor que la que se ofrece:

                for(McoAppRegister& iter_app : my_list){

                    if(iter_app.traffic_class_ == i){
                        
                        std::cout << "El intervalo de la aplicacion de puerto " << iter_app.PORT_ << " se modificó a: " << iter_app.min_interval << std::endl;
                        iter_app.interval_ = std::chrono::microseconds(iter_app.min_interval);

                        ACRi -= fraction_time;

                    }

                }

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

                    if((iter_app.traffic_class_ == i) && (iter_app.size_average > 0)){ 
                        //CUIDADO!! si la lista de estadisticas de mensajes se resetea entera antes de este set
                        //esa aplicacion se saltará este control
                        
                        double CREij = ((iter_app.size_average / data_speed) / (iter_app.interval_average)); //us / us
                        //Esto tal vez mejor almacenarlo en el registro de la aplicacion en el anterior for para no tener que calcular el valor 2 veces 

                        double ACRij = (CREij / CRi) * ACRi;

                        unsigned Tonij = iter_app.size_average / data_speed; //us

                        unsigned Toffij = Tonij *((1 - ACRij) / ACRij); //us

                        iter_app.interval_ = std::chrono::microseconds(Tonij + Toffij);

                    }

                }

                ACRi = 0;
                
            }

        }

    }
    
}

int McoFac::rand_traffic_class(){

    srand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return rand() % 4; 

}

Application* McoFac::search_port(vanetza::btp::port_type PORT){

    Application* aplication = nullptr;

    for(McoAppRegister& iter : my_list){

        if(iter.application_.port() == PORT){

            aplication = &iter.application_;

            break;

        }

    }

    return aplication;
}

void McoFac::byte_counter_update(unsigned packet_size){

    const unsigned network_header = 0;

    const unsigned data_link_header = 0;

    const unsigned transport_header = 0;

    const unsigned header_size = network_header + data_link_header + transport_header;

    packet_size += header_size;

    byte_counter += packet_size;

}

void McoFac::CBR_update(){

    const double data_speed = 0.75; // bytes/microseconds

    double time_ocuped = byte_counter / data_speed; //microseconds

    CBR = ((time_ocuped / mco_interval_.count()) + CBR) * 0.5;

    std::cout << "CBR: " << CBR << std::endl;

    byte_counter = 0;

}

void McoFac::set_min_interval(){

    for(auto& iter_app : my_list){

        iter_app.min_interval = iter_app.interval_.count();

    }

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

void McoFac::set_traffic_class(int traffic_class){

    auto& last_app = my_list.back();
    
    if(traffic_class < 4 && traffic_class >= 0){

        last_app.traffic_class_ = traffic_class;
    
    } else{

        last_app.traffic_class_ = 3;

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
    Application* application = search_port(indication.destination_port);

    if(indication.destination_port == btp::ports::CAM || indication.destination_port == btp::ports::CAM1 || indication.destination_port == btp::ports::CAM2
     || indication.destination_port == btp::ports::CAM3 || indication.destination_port == btp::ports::CAM4){

        asn1::PacketVisitor<asn1::Cam> visitor;
        std::shared_ptr<const asn1::Cam> cam = boost::apply_visitor(visitor, *packet);
        byte_counter_update(cam->size());
    }

    application->indicate(indication, std::move(packet));

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

    calc_adapt_delta();

    CBR_update();

    set_adapt_interval();



    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}