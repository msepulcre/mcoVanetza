#!/bin/bash

cd /home/yeray/proyectos/mcoVanetza/tools/socktap/test

if [ "$2" == "a" ];then
    sudo ./conteneriza.sh
fi

for((i = 1; i <= $1; i++)); do
    
    sudo docker rm socktap$i || echo No hay contenedores con el nombre socktap$i

done

if [ "$1" == "1" ]; then

    sudo docker run -ti -v /usr/local/src/socktap1:/usr/local/src/socktap --name socktap --network bridge socktap-docker

else

    for((i = 1; i <= $1; i++)); do
    
        sudo docker run -d -v /usr/local/src/socktap$1:/usr/local/src/socktap --name socktap$i --network bridge socktap-docker

    done

fi