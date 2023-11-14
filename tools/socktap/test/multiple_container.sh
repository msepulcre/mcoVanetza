#!/bin/bash

cd /home/yeray/proyectos/mcoVanetza/tools/socktap/test

if [ "$2" == "a" ];then
    sudo ./conteneriza.sh

else

    for((i = 1; i <= $1; i++)); do
    
        sudo docker stop socktap$i

    done

fi



for((i = 1; i <= $1; i++)); do
    
    sudo docker rm socktap$i || echo No hay contenedores con el nombre socktap$i

done

if [ "$1" == "1" ]; then

    sudo docker run -ti -v /usr/local/src/socktap1:/usr/local/src/socktap --name socktap --network bridge socktap-docker

else

    for((i = 1; i <= $1; i++)); do
    
        sudo docker run -d -v /usr/local/src/socktap$i:/usr/local/src/socktap --name socktap$i --network bridge socktap-docker


    done

    if [ "$3" == "t" ]; then

        for((i = 1; i <= $1; i++)); do
    
            gnome-terminal --tab --title="Termianl de Socktap$i" -- bash -c "sudo docker logs -f socktap$i; exec bash"

        done
    fi

fi

if [ "$4" == "l" ]; then

    sudo sleep 300
    for((i = 1; i <= $1; i++)); do
    
        sudo docker cp socktap$i:/home/build-user/inpercept_log.log /home/yeray/Documentos/ipercept$i_log.log
    
    done
    
fi