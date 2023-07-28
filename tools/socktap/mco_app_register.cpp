#include "mco_app_register.hpp"
#include <iostream>
#include <cstring>
#include <string.h>


McoAppRegister::McoAppRegister(){

    app_name = "";
    msgSize = 0;
    msgInterval = 0;

}

McoAppRegister::McoAppRegister(std::string cadena){
    
    app_name.assign(cadena);
    msgSize = 0;
    msgInterval = 0;

}


McoAppRegister::McoAppRegister(McoAppRegister *appCopied){

    app_name.assign(appCopied->app_name);
    msgSize = appCopied->msgSize;
    msgInterval = appCopied->msgInterval;

}

McoAppRegister::McoAppRegister(std::string cadena, float size, float interval){

    app_name.assign(cadena);
    msgSize = size;
    msgInterval = interval;

}