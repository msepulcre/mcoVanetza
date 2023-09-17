#include "mco_fac.hpp"
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


// This is a very simple CA application sending CAMs at a fixed rate.

using namespace vanetza;
using namespace vanetza::facilities;
using namespace std::chrono;

McoFac::McoFac(PositionProvider& positioning, Runtime& rt) :
    positioning_(positioning), runtime_(rt), mco_interval_(milliseconds(100)), adapt_delta(1)
{
    schedule_timer();
}

McoFac::DataConfirm McoFac::mco_data_request(const DataRequest& request, DownPacketPtr packet, std::string app_name, PortType PORT) //YERAY
{
    
    DataConfirm confirm(DataConfirm::ResultCode::Rejected_Unspecified);

    byte_counter  += packet->size();

    register_packet(app_name, packet->size(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    
    confirm = Application::request(request, std::move(packet), PORT);
    return confirm;
}

void McoFac::register_packet(std::string app_name, float msgSize, int64_t msgTime ){

    McoAppRegister* app_registered;

    for(McoAppRegister&iter : my_list){

        if(iter.app_name == app_name){
            
            app_registered = &iter;
            break;

        }

    }

    app_registered->msg_data_list.push_back({msgSize, msgTime});

    std::cout << "El tamaño de la lista de datos de " << app_registered->app_name << " es " 
    << app_registered->msg_data_list.size() << std::endl;
    

}

std::string McoFac::rand_name(){

    char strrnd[10];
    srand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    for(int i=0; i <= 9; i++){
        strrnd[i] = 33 + rand() % (126 - 33);
    }
    return strrnd;

}

bool McoFac::search_in_list(std::string app_name){
    for(auto iter : my_list){

        if(iter.app_name == app_name){

            return true;
        }

    }
    return false;

}

std::string McoFac::register_app(vanetza::Clock::duration& interval_,  Application& application){ 

    bool name_used;
    std::string app_name;
    
    do{
        app_name = rand_name();
        name_used = search_in_list(app_name);

        if(!name_used){
            
            
            my_list.push_back(McoAppRegister(app_name, interval_, rand_traffic_class(), application));
            std::cout << "Se ha registrado la aplicacion con el nombre: " << app_name << std::endl;

        } else{

            std::cout << "El nombre "<< app_name <<" ya esta en uso" << std::endl;
        }

    }while(name_used);

    std::cout << "El numero de aplicaciones registradas es: " << my_list.size() << std::endl;

    return app_name;

}

void McoFac::clean_outdated(){

    const float delated_time = 5000;
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

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

    const double data_speed = 0.75 * 1000000; // byte/s

    if(my_list.size() != 0){

        int apps_number[4] = {0 , 0, 0, 0}; //numero de aplicaciones de cada traffic class

        for(McoAppRegister& iter_app : my_list){

            apps_number[iter_app.traffic_class_]++;
        }

        for(int i = 0; (i < 4) && (adapt_delta != 0) ; i++){

            float fraction_time = 0;
            
            for(auto iter_app : my_list){

                if(iter_app.traffic_class_ = i){

                    fraction_time += ((iter_app.size_average / data_speed) / (iter_app.min_interval * 1000000)); // s / s
                    // fraccion de tiempo que la clase i pide
                }

            }

            if(fraction_time <= adapt_delta){ //si la fraccion de tiempo que se pide es menor que la que se ofrece:

                for(McoAppRegister& iter_app : my_list){

                    if(iter_app.traffic_class_ = i){
                        
                        std::cout << "El intervalo de la aplicacion " << iter_app.app_name << " se modificó a: " << iter_app.min_interval << std::endl;
                        iter_app.interval_ = std::chrono::microseconds(iter_app.min_interval);

                        adapt_delta -= fraction_time;

                    }

                }

            } else{ //si es mayor

                adapt_delta /= apps_number[i];

                for(McoAppRegister& iter_app : my_list){

                    if((iter_app.traffic_class_ = i) && (iter_app.size_average > 0)){

                        unsigned Ton = iter_app.size_average / data_speed;
                        unsigned Toff = Ton / adapt_delta;
                        
                        std::cout << "El intervalo de la aplicacion " << iter_app.app_name << " se modificó a: " << Ton + Toff << std::endl;
                        iter_app.interval_ = std::chrono::seconds(Ton + Toff);

                    }

                }
                adapt_delta = 0;
                
            }

        }

    }
    
}

int McoFac::rand_traffic_class(){

    srand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return rand() % 4; 

}

int McoFac::search_traffic_class(std::string app_name){

    for(auto iter : my_list){

        if(app_name ==  iter.app_name){

            return iter.traffic_class_;
        }


    }

    return 3;
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

void McoFac::CBR_update(){

    const double data_speed = 0.75 * 1000000000000; // bytes/microseconds

    double time_ocuped = byte_counter / data_speed;

    CBR = time_ocuped / mco_interval_.count();

    byte_counter = 0;

}

void McoFac::byte_counter_update(unsigned packet_size){

    const unsigned network_header = 0;

    const unsigned data_link_header = 0;

    const unsigned transport_header = 0;

    const unsigned header_size = network_header + data_link_header + transport_header;

    packet_size += header_size;

    byte_counter += packet_size;

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
    
    asn1::PacketVisitor<asn1::Cam> visitor;
    std::shared_ptr<const asn1::Cam> cam = boost::apply_visitor(visitor, *packet);

    byte_counter_update(cam->size());

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

    /* set_adapt_interval(); */



    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}