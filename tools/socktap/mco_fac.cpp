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
    confirm = Application::request(request, std::move(packet));
    return confirm;
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

            McoAppRegister *myMcoApp = new McoAppRegister(app_name);
            my_list.push_back(myMcoApp);
            std::cout << "Se ha registrado la aplicacion con el nombre: " << app_name << std::endl;

        } else{

            std::cout << "El nombre "<< app_name <<" ya esta en uso" << std::endl;
        }

    }while(name_used);

    std::cout << "El numero de aplicaciones registradas es: " << my_list.size() << std::endl;

    return app_name;

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

    // clean
    //  cbr = x => delta = y
    // calc average size and interval
    // decidir si cada servicio tiene que subir o bajar su tasa y decirselo
}