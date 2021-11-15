FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow

RUN apt-get update && apt-get install -y \
    libboost-all-dev \
    libgflags-dev \
    libgoogle-glog-dev \
    libzmq3-dev \
    qt5-default
COPY server/cmake-build-debug/server /server
RUN chmod +x /server
ENTRYPOINT ./server
