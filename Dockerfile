# windows-games
#
# VERSION               1.0

FROM ubuntu:26.04
MAINTAINER Werner R. Mendizabal "nonameentername@gmail.com"

RUN apt-get update --fix-missing
RUN apt-get install -y make build-essential pkg-config libsdl2-dev git python3

RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR emsdk
RUN ./emsdk install 3.1.64
RUN ./emsdk activate 3.1.64

ENV EMSDK=/emsdk
ENV EM_CONFIG=/emsdk/.emscripten
ENV EM_CACHE=/emsdk/upstream/emscripten/cache
ENV PATH=/emsdk:/emsdk/upstream/emscripten:$PATH

WORKDIR /source
