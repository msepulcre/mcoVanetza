#include "mco_app_register.hpp"
#include <iostream>
#include <cstring>
#include <string.h>


McoAppRegister::McoAppRegister(){

    app_name = "";
    msg_data_list.push_back( { 0 , 0 });
    size_average = 0;
    interval_average = 0;

}

McoAppRegister::McoAppRegister(std::string cadena){
    
    app_name.assign(cadena);
    msg_data_list.push_back({ 0 , 0});
    size_average = 0;
    interval_average = 0;


}


McoAppRegister::McoAppRegister(McoAppRegister *appCopied){

    app_name.assign(appCopied->app_name);
    
    for(MsgData &iter : appCopied->msg_data_list){

        msg_data_list.push_back({ iter.msgSize , iter.msgTime});

    }

    size_average = appCopied->size_average;
    interval_average = appCopied->interval_average;

}

McoAppRegister::McoAppRegister(std::string cadena, float size, int64_t time){

    app_name.assign(cadena);
    msg_data_list.push_back({size , time});
    size_average = size;
    interval_average = 0;

}

