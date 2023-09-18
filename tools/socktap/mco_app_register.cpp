#include "mco_app_register.hpp"
#include <iostream>
#include <cstring>
#include <string.h>


McoAppRegister::McoAppRegister(vanetza::Clock::duration& interval, Application& application):
interval_(interval), min_interval(interval_.count()), application_(application)
{

    size_average = 0;
    interval_average = 0;

}

McoAppRegister::McoAppRegister(PortType PORT, vanetza::Clock::duration& interval, int traffic_class, Application& application):
PORT_(PORT) ,interval_(interval), traffic_class_(traffic_class), min_interval(interval_.count()), application_(application)
{   
    size_average = 0;
    interval_average = 0;

} //Se usa esta 


McoAppRegister::McoAppRegister(McoAppRegister *appCopied):
interval_(appCopied->interval_), min_interval(interval_.count()), application_(appCopied->application_)
{

    PORT_ = appCopied->PORT_;
    
    for(MsgData &iter : appCopied->msg_data_list){

        msg_data_list.push_back({ iter.msgSize , iter.msgTime});

    }

    size_average = appCopied->size_average;
    interval_average = appCopied->interval_average;

}

McoAppRegister::McoAppRegister(PortType PORT, float size, int64_t time, vanetza::Clock::duration& interval, Application& application):
interval_(interval), min_interval(interval_.count()), application_(application)
{

    PORT_ = PORT;
    msg_data_list.push_back({size , time});
    size_average = size;
    interval_average = 0;

}

