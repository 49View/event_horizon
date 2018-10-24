# Install boost

# Example: $1=1 $2=67 $3=0 

apt-get update
apt-get -y install build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev libboost-all-dev

wget https://dl.bintray.com/boostorg/release/$1.$2.$3/source/boost_$1_$2_$3.tar.gz 
tar -xvzf boost_$1_$2_$3.tar.gz
./boost_$1_$2_$3/bootstrap.sh --prefix=/usr/local/
cd boost_$1_$2_$3
./b2 
./b2 install
