#!/bin/bash


for((i = 1; i <= $1; i++)); do
    
    sudo docker stop socktap$i || echo No hay contenedores con el nombre socktap$i

    if [ "$2" == "2" ]; then

        sudo docker rm socktap$i

    fi
    

done

