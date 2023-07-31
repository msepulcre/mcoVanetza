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
    std::cout << "Tamaño del paquete: " << packet->size() << std::endl;

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
    
    
    //buscar en my_list el registro de la aplicacion y luego añadir a la lista de datos los datos actuales

}

McoFac::McoFac(PositionProvider& positioning, Runtime& rt) :
    positioning_(positioning), runtime_(rt), cam_interval_(seconds(1))
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
    for(auto aplicacion : my_list){

        if(aplicacion.app_name == app_name){

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

    //asumo que tengo que sacar el tiempo de mi ordenador y no el tiempo que marca alguna funcionn de vanetza
    //de la forma en la que he hecho el codigo, current time tiene un error creciente por cada iteracion, lo hago de otra forma?

    const float delated_time = 5000;
    auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::cout << "El tiempo actual es: " << current_time << std::endl;


    for(auto iter_app = my_list.begin() ; iter_app != my_list.end() ; iter_app++ ){

        std::cout << "Hay " << iter_app->msg_data_list.size() << " msg_data_register en la aplicacion" << iter_app->app_name << std::endl;

        for(auto iter_data = iter_app->msg_data_list.begin() ; iter_data != iter_app->msg_data_list.end() ;){

            if(current_time - iter_data->msgTime > delated_time){

                iter_data = iter_app->msg_data_list.erase(iter_data);
                std::cout << "Se ha eliminado 1 dato de msg_data_registered" << std::endl;

            }
            else {

                ++iter_data;

            }

        }

        std::cout << "Quedan " << iter_app->msg_data_list.size() << " msg_data_register en la aplicacion" << iter_app->app_name << std::endl;
    }

}

void McoFac::set_interval(Clock::duration interval)
{
    cam_interval_ = interval;
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
    runtime_.schedule(cam_interval_, std::bind(&McoFac::on_timer, this, std::placeholders::_1), this);
}

void McoFac::on_timer(Clock::time_point)
{
    
    schedule_timer();

    clean_outdated();


    



    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}