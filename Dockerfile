FROM debian:latest


WORKDIR /home
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get --yes update && \
    apt-get --yes install clang \ 
        libboost-filesystem-dev \ 
        libboost-system-dev \ 
        libcrypto++-dev \ 
        libstdc++6 \
        make \
        git && \
    git clone https://github.com/rust-lang/book.git test