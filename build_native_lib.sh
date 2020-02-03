docker build -t $1/lib$2 -f ./native/dockerfile-lib --build-arg LIB_NAME=$2 ./native
