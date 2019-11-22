#!/bin/bash

LIBNAME_NAME="$1"
LIBNAME_VERSION="$2"
GITUSER="$3"
OPTIONS=$4

docker build -t sixthview/$LIBNAME_NAME-package -f dockerfile-packetbuilder.docdev --build-arg LIBNAME_NAME=$LIBNAME_NAME --build-arg LIBNAME_VERSION=$LIBNAME_VERSION --build-arg GITUSER=$GITUSER --build-arg OPTIONS="$OPTIONS" .
docker push sixthview/$LIBNAME_NAME-package