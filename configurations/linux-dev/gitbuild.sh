unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

LIBNAME_NAME="$1"
LIBNAME_VERSION="$2"
GITUSER="$3"
OPTIONS=$4
PROJECTLIBS=$5

BUILDSYSTEM=""

MAINBUILDINGDIR="/3rdpartylibs/"
if [ ${machine} == "MinGw" ]; then
	MAINBUILDINGDIR="c:/3rdpartylibs/"	
	BUILDSYSTEMIMPL="MinGW Makefiles"
	BUILDSYSTEM="-G""$BUILDSYSTEMIMPL"
	OPTIONS="$OPTIONS"" -DCMAKE_INSTALL_PREFIX="c:/usr/local" -DCMAKE_SH="CMAKE_SH-NOTFOUND""
	export CXXFLAGS=-isystem\ c:/usr/local/include
	export LDFLAGS="-Lc:/usr/local/lib"
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:c:/usr/local/lib
fi

mkdir $MAINBUILDINGDIR
cd $MAINBUILDINGDIR
rm -rf $LIBNAME_NAME
mkdir $LIBNAME_NAME
cd $LIBNAME_NAME/
git clone --recursive --branch $LIBNAME_VERSION https://github.com/$GITUSER/$LIBNAME_NAME
LIB_INTERNALBUILD_DIR=$LIBNAME_NAME-build
mkdir $LIB_INTERNALBUILD_DIR
cd $LIB_INTERNALBUILD_DIR

cmake $OPTIONS " " "$BUILDSYSTEM" " " -DCMAKE_BUILD_TYPE=Release ../$LIBNAME_NAME
make -j8
make install
