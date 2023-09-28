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

    DataConfirm mco_data_request(const DataRequest&, DownPacketPtr, PortType PORT); 

    void register_app(PortType PORT ,vanetza::Clock::duration& interval_, Application& application);

    void register_packet(PortType PORT, float msgSize, int64_t msgTime);
    
    void clean_outdated();

    void apps_average_size();
    
    void apps_average_interval();

    void calc_adapt_delta();

    void set_adapt_interval();

    int rand_traffic_class();

    Application* search_port(vanetza::btp::port_type PORT);

    void byte_counter_update(unsigned packet_size);

    void CBR_update();

    /* void list_counter_update(unsigned packet_size);

    void CBR_update2(); */

    void set_min_interval();

    void set_apps_number();

    double adapt_delta;

    double CBR_target;

    double CBR;

    unsigned byte_counter;

    int apps_number[4] = {0, 0, 0, 0};
    
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;
    void set_interval(vanetza::Clock::duration);
    void print_received_message(bool flag);
    void print_generated_message(bool flag);

    std::list<McoAppRegister> my_list;

/* protected:
    
    vanetza::geonet::Router* router_;
    vanetza::geonet::GbcDataRequest request_gbc(const DataRequest&);
    vanetza::geonet::ShbDataRequest request_shb(const DataRequest&); */
    
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
