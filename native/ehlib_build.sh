#!/bin/bash

LIBNAME_NAME="$1"

docker build -t ateventhorizon/lib$LIBNAME_NAME -f dockerfile-lib --build-arg LIBNAME_NAME=$LIBNAME_NAME .

echo "$DOCKER_HUB_TOKEN" | docker login -u ateventhorizon --password-stdin
docker push ateventhorizon/lib$LIBNAME_NAME
