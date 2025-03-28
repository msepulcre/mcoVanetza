#ifndef MCO_APP_REGISTER_HPP_PSIGPUTG
#define MCO_APP_REGISTER_PSIGPUTG

#include "application.hpp"
#include <vanetza/btp/ports.hpp>
#include <vanetza/common/clock.hpp>
#include <iostream>
#include <string.h>
#include <cstring>
#include <list>


class McoAppRegister
{
public:

    using PortType = vanetza::btp::port_type;
    
    PortType PORT_;
    
    struct MsgData{

        float msgSize; //bytes
        int64_t msgTime; //microseconds

    };

    double size_average;

    int64_t interval_average;

    vanetza::Clock::duration& interval_;

    int traffic_class_;

    int64_t min_interval; //microsegundos

    std::list<MsgData> msg_data_list;

    Application& application_;
    
    McoAppRegister(vanetza::Clock::duration& interval, Application& application);
    McoAppRegister(PortType PORT, vanetza::Clock::duration& interval, Application& application);
    McoAppRegister(McoAppRegister *appCopied);
    McoAppRegister(PortType PORT, float size, int64_t time, vanetza::Clock::duration& interval, Application& application);

};


#endif 