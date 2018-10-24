APPVERSION=$1
wget https://github.com/embree/embree/releases/download/v$APPVERSION/embree-$APPVERSION.x86_64.linux.tar.gz
tar xzf embree-$APPVERSION.x86_64.linux.tar.gz
source embree-$APPVERSION.x86_64.linux/embree-vars.sh
