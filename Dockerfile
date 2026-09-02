# windows-games
#
# VERSION               1.0

FROM ubuntu:26.04
MAINTAINER Werner R. Mendizabal "nonameentername@gmail.com"

RUN apt-get update --fix-missing
RUN apt-get install -y make build-essential libsdl2-dev

WORKDIR /source
