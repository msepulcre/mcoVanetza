cd ../../../build
sudo make socktap

cp -p bin/socktap ../tools/docker/
cd ../tools/docker/

sudo docker stop $(docker ps -aq)
#sudo docker rm inpercept_id100
#sudo docker rm inpercept_id200
sudo docker rmi socktap-docker

sudo docker system prune -a -f

sudo docker build -t socktap-docker .