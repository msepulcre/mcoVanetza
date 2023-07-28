#ifndef MCO_APP_REGISTER_HPP_PSIGPUTG
#define MCO_APP_REGISTER_PSIGPUTG

#include <iostream>
#include <string.h>
#include <cstring>

class McoAppRegister
{
public:
    
    std::string app_name;
    float msgSize;
    float msgInterval;

    McoAppRegister();
    McoAppRegister(std::string cadena);
    McoAppRegister(McoAppRegister *appCopied);
    McoAppRegister(std::string cadena, float size, float interval);

};


#endif 