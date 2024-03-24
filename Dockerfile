FROM ubuntu:latest

RUN apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install tzdata
ENV LANG en_US.utf8

RUN apt-get install -y build-essential \
    cmake ninja-build vim \
    clang-tidy git wget

RUN apt install -y python3-pip
RUN pip install grequests

RUN mkdir -p /workspace/conan 
ENV CONAN_HOME=/workspace/conan

RUN git clone https://github.com/conan-io/conan conan_src && \
	cd conan_src && \
	git fetch --all && \
	git checkout -b develop3 origin/develop2 && \
	python3 -m pip install -e .
	
RUN conan profile detect --force && \
	grep -v compiler.cppst $CONAN_HOME/profiles/default > $CONAN_HOME/profiles/default.tmp && \
	echo compiler.cppstd=20 >> $CONAN_HOME/profiles/default.tmp && \
 	cp $CONAN_HOME/profiles/default.tmp $CONAN_HOME/profiles/default && \
	rm -f $CONAN_HOME/profiles/default.tmp && \
	chmod -R 0777 /workspace/conan

