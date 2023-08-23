#ifndef MCO_APP_REGISTER_HPP_PSIGPUTG
#define MCO_APP_REGISTER_PSIGPUTG

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

        float msgSize;
        int64_t msgTime;  

    };

    float size_average;

    float interval_average;

    vanetza::Clock::duration& interval_;

    int traffic_class_;

    std::list<MsgData> msg_data_list;
    
    McoAppRegister(vanetza::Clock::duration& interval);
    McoAppRegister(std::string cadena, vanetza::Clock::duration& interval, int traffic_class);
    McoAppRegister(McoAppRegister *appCopied);
    McoAppRegister(std::string cadena, float size, int64_t time, vanetza::Clock::duration& interval);

};


#endif 