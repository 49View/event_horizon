apt-get install -y default-jre
cd /3rdpartylibs
git clone https://github.com/juj/emsdk.git
cd emsdk/
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

#install ZLIB port, for some reason on linux is not generated with -s USE_ZLIB=1
mkdir /zlib_em
cd /zlib_em
git clone https://github.com/madler/zlib.git
cd zlib/
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/3rdpartylibs/emsdk/emscripten/1.38.6/cmake/Modules/Platform/Emscripten.cmake .
make -j8
make install
rm -rf /zlib_em
