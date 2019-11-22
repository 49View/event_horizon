#!/bin/bash

P1="$1"
P2="$2"
P3="$3"
LIBNAME_NAME=boost

docker build -t sixthview/$LIBNAME_NAME-package -f dockerfile-packetbuilder-boost.docdev --build-arg P1=$P1 --build-arg P2=$P2 --build-arg P3=$P3 .
docker push sixthview/$LIBNAME_NAME-package