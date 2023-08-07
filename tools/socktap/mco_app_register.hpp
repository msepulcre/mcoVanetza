#ifndef MCO_APP_REGISTER_HPP_PSIGPUTG
#define MCO_APP_REGISTER_PSIGPUTG

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

    std::list<MsgData> msg_data_list;
    
    McoAppRegister();
    McoAppRegister(std::string cadena);
    McoAppRegister(McoAppRegister *appCopied);
    McoAppRegister(std::string cadena, float size, int64_t interval);

};


#endif 