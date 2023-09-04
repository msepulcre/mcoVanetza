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

McoFac::DataConfirm McoFac::mco_data_request(const DataRequest& request, DownPacketPtr packet, std::string app_name) //YERAY
{
    
    DataConfirm confirm(DataConfirm::ResultCode::Rejected_Unspecified);

    register_packet(app_name, packet->size(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    
    waiting_queue(app_name);

    confirm = Application::request(request, std::move(packet));
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

std::string McoFac::register_app(vanetza::Clock::duration& interval_){ 

    bool name_used;
    std::string app_name;
    
    do{
        app_name = rand_name();
        name_used = search_in_list(app_name);

        if(!name_used){
            
            
            my_list.push_back(McoAppRegister(app_name, interval_, rand_traffic_class()));
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
    
}

void McoFac::set_adapt_interval(){

    const double data_speed = 0.006; // Mbits/mseg

    //esto igual convendria ponerlo tambien en el registro y que lo dé el constructor de
    //cada app, para que cada una tenga su velocidad

    if(my_list.size() != 0){

        float relative_adapt_delta = adapt_delta/my_list.size(); //divido delta por el numero de aplicaciones en marcha

        for(McoAppRegister& iter_app : my_list){
            
            if(iter_app.size_average != 0){

                double Ton = (iter_app.size_average / data_speed)*0.008; //aqui asumo que packet->size() da bytes y lo paso a Mbits
                unsigned Toff = Ton/relative_adapt_delta;

                std::cout << "Se modifico el intervalo a: " << Toff << std::endl; 
                iter_app.interval_ = std::chrono::milliseconds(Toff);  
            }
        }
    }
    

}

int McoFac::rand_traffic_class(){

    srand(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    return rand() % 4; 

}

void McoFac::waiting_queue(std::string app_name){

    int traffic_class = search_traffic_class(app_name);

    std::string packet_name = rand_name();

    packet_queue[traffic_class].push_back({app_name, packet_name});

    bool first_in_queue = 0;

    while(first_in_queue == 0){

        auto iter = packet_queue[traffic_class].begin();
        
        if((iter->app_name == app_name) && (iter->packet_name == packet_name)){

            first_in_queue = 1;
          
        }

        
    }
    
    bool no_others_queue = 0;

    if(traffic_class != 0){

        while(no_others_queue == 0){

            no_others_queue = 1;

            for(int i = traffic_class -1; i >= 0; i--){

                if(!packet_queue[i].empty()){

                    no_others_queue = 0;

                }

            }

        }
    }
    //El paquete ya no tiene que esperar y se borra de la cola

    bool deleted_item = 0;

    for(auto iter = packet_queue[traffic_class].begin(); iter != packet_queue[traffic_class].end();){

        if((iter->app_name == app_name) && (iter->packet_name == packet_name)){

            iter = packet_queue[traffic_class].erase(iter);
            break;
          
        } else{
            
            iter++;

        }


    }
}

int McoFac::search_traffic_class(std::string app_name){

    for(auto iter : my_list){

        if(app_name ==  iter.app_name){

            return iter.traffic_class_;
        }


    }

    return 3;
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
    return btp::ports::CAM;
}

void McoFac::indicate(const DataIndication& indication, UpPacketPtr packet)
{
    asn1::PacketVisitor<asn1::Cam> visitor;
    std::shared_ptr<const asn1::Cam> cam = boost::apply_visitor(visitor, *packet);

    std::cout << "CAM application received a packet with " << (cam ? "decodable" : "broken") << " content" << std::endl;
    if (cam && print_rx_msg_) {
        std::cout << "Received CAM contains\n";
        print_indented(std::cout, *cam, "  ", 1);
    }
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

    set_adapt_interval();


    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}