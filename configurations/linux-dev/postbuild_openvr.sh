APPVERSION=$1

mkdir /usr/local/include/openvr
mkdir /usr/local/include/openvr/shared
cp -rf /3rdpartylibs/openvr/openvr-$APPVERSION/headers/* /usr/local/include/openvr
cp -rf /3rdpartylibs/openvr/openvr-$APPVERSION//samples/shared/*.h* /usr/local/include/openvr/shared
