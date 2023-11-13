FROM ubuntu:latest

RUN DEBIAN_FRONTEND=noninteractive apt-get -y install tzdata
ENV LANG en_US.utf8

RUN apt-get install -y build-essential cmake \
    vim libgtest-dev ninja-build

