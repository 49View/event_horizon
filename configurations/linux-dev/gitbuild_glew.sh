# RapidJSON
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
cd $LIBNAME_NAME-$LIBNAME_VERSION
make extensions
make -j8
make install
make clean
