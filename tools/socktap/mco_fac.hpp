#ifndef MCO_FAC_HPP_PSIGPUTG
#define MCO_FAC_HPP_PSIGPUTG

#include "mco_app_register.hpp"
#include "application.hpp"
#include <iostream>
#include <list>
#include <cstring>
#include <string.h>
#include <chrono>
#include <vanetza/common/clock.hpp>
#include <vanetza/common/position_provider.hpp>
#include <vanetza/common/runtime.hpp>

class McoFac : public Application
{
public:

    using DataRequest = vanetza::btp::DataRequestGeoNetParams;
    using DataConfirm = vanetza::geonet::DataConfirm;
    using DownPacketPtr = vanetza::geonet::Router::DownPacketPtr;

    
    McoFac(vanetza::PositionProvider& positioning, vanetza::Runtime& rt); 

    bool search_in_list(std::string app_name);

    std::string register_app(vanetza::Clock::duration& interval_, Application& application);

    void register_packet(std::string app_name, float msgSize, int64_t msgTime);

    std::string rand_name();
    
    void clean_outdated();

    void apps_average_size();
    
    void apps_average_interval();

    void calc_adapt_delta();

    void set_adapt_interval();

    int rand_traffic_class();

    int search_traffic_class(std::string app_name);

    Application* search_port(vanetza::btp::port_type PORT);

    void byte_counter_update(unsigned packet_size);

    void CBR_update();

    double adapt_delta;

    double CBR_target;

    double CBR;

    unsigned byte_counter;
    
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;
    void set_interval(vanetza::Clock::duration);
    void print_received_message(bool flag);
    void print_generated_message(bool flag);
    DataConfirm mco_data_request(const DataRequest&, DownPacketPtr, std::string app_name, PortType PORT); 

    std::list<McoAppRegister> my_list;

/* protected:
    
    vanetza::geonet::Router* router_;
    vanetza::geonet::GbcDataRequest request_gbc(const DataRequest&);
    vanetza::geonet::ShbDataRequest request_shb(const DataRequest&);
//Esto para que? */
    
private:
    void schedule_timer();
    void on_timer(vanetza::Clock::time_point);

    vanetza::PositionProvider& positioning_;
    vanetza::Runtime& runtime_;
    vanetza::Clock::duration mco_interval_;
    bool print_rx_msg_ = false;
    bool print_tx_msg_ = false;
};

#endif /* MCO_FAC_HPP_EUIC2VFR */
