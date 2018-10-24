CMAKE_VERSION=$1.$2.$3
CMAKE_VERSION_SHORT=$1.$2

wget https://cmake.org/files/v${CMAKE_VERSION_SHORT}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh /cmake-${CMAKE_VERSION}-Linux-x86_64.sh
mkdir /opt/cmake
sh ./cmake-${CMAKE_VERSION}-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
