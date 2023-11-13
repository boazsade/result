FROM ubuntu:latest

RUN DEBIAN_FRONTEND=noninteractive apt-get -y install tzdata
ENV LANG en_US.utf8

RUN apt-get install -y build-essential cmake \
    vim libgtest-dev ninja-build

RUN mkdir /build
WORKDIR /build
COPY result.h /build
COPY result_ut.cpp /build
COPY CMakeLists.txt /build
