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

McoFac::DataConfirm McoFac::mco_data_request(const DataRequest& request, DownPacketPtr packet, std::string app_name) //YERAY
{
    
    DataConfirm confirm(DataConfirm::ResultCode::Rejected_Unspecified);

    register_packet(app_name, packet->size(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    
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

McoFac::McoFac(PositionProvider& positioning, Runtime& rt) :
    positioning_(positioning), runtime_(rt), mco_interval_(milliseconds(100))
{
    schedule_timer();
}

std::string McoFac::rand_name(){

    char strrnd[10];
    srand(time(NULL) + my_list.size());
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

std::string McoFac::register_app(){ 

    bool name_used;
    std::string app_name;
    
    do{
        app_name = rand_name();
        name_used = search_in_list(app_name);

        if(!name_used){

            my_list.push_back(McoAppRegister(app_name));
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


    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}