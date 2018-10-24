APPVERSION=$1

mkdir /GitFS
cd GitFS
wget https://github.com/git-lfs/git-lfs/releases/download/v$APPVERSION/git-lfs-linux-386-$APPVERSION.tar.gz 
tar -zxvf git-lfs-linux-386-$APPVERSION.tar.gz 
set -eu
mkdir -p /usr/local/bin	
ls /GitFS
install /GitFS/git-lfs-$APPVERSION/git-lfs "/usr/local/bin/git-lfs"
PATH=$PATH:/usr/local/bin
rm -r /GitFS
