ARG UBUNTU_VERSION=latest
FROM ubuntu:$UBUNTU_VERSION
ARG DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]
COPY socktap /usr/local/bin/socktap
RUN apt-get update && \
apt-get install -y --no-install-recommends \
sudo \
libboost-date-time-dev \
libboost-program-options-dev \
libboost-system-dev \
libcrypto++-dev \
libgeographic-dev \
libssl-dev \
libjsoncpp-dev \
libmosquitto-dev \
libgps-dev \
liblog4cxx-dev \
&& rm -rf /var/lib/apt/lists/*
RUN useradd -m build-user
USER build-user
WORKDIR /home/build-user
ARG ID
USER $ID
ENTRYPOINT sudo socktap -a ca -a static -i eth0 --print-tx-cam