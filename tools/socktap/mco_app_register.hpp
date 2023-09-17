#ifndef MCO_APP_REGISTER_HPP_PSIGPUTG
#define MCO_APP_REGISTER_PSIGPUTG

#include "application.hpp"
#include <vanetza/common/clock.hpp>
#include <iostream>
#include <string.h>
#include <cstring>
#include <list>

class McoAppRegister
{
public:
    
    std::string app_name;
    
    struct MsgData{

        float msgSize; //bytes
        int64_t msgTime;  

    };

    float size_average;

    float interval_average;

    vanetza::Clock::duration& interval_;

    int traffic_class_;

    const unsigned min_interval; //microsegundos

    std::list<MsgData> msg_data_list;

    Application& application_;
    
    McoAppRegister(vanetza::Clock::duration& interval, Application& application);
    McoAppRegister(std::string cadena, vanetza::Clock::duration& interval, int traffic_class, Application& application);
    McoAppRegister(McoAppRegister *appCopied);
    McoAppRegister(std::string cadena, float size, int64_t time, vanetza::Clock::duration& interval, Application& application);

};


#endif 