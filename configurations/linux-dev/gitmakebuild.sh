LIBNAME_NAME="$1"
LIBNAME_VERSION="$2"
GITUSER="$3"
OPTIONS=$4 #"-DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF "
PROJECTLIBS=$5

mkdir /3rdpartylibs/
cd /3rdpartylibs/
rm -rf $LIBNAME_NAME
mkdir $LIBNAME_NAME
cd $LIBNAME_NAME/
wget https://github.com/$GITUSER/$LIBNAME_NAME/archive/$LIBNAME_VERSION.tar.gz
tar -xvzf $LIBNAME_VERSION.tar.gz 
echo "Changin folder to " $LIBNAME_NAME-${LIBNAME_VERSION//v}
cd $LIBNAME_NAME-${LIBNAME_VERSION//v}
echo "Changin folder to " $LIBNAME_NAME
cd $LIBNAME_NAME
./autogen.sh
./configure
make -j8
make install
