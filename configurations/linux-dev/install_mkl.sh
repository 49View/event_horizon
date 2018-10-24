APPVERSION=$1 #2018.2.199
wget https://s3.amazonaws.com/sixthviewfs/libs/mkl/linux/l_mkl_$APPVERSION.tgz
tar -xvzf l_mkl_$APPVERSION.tgz
rm l_mkl_$APPVERSION/silent.cfg
wget https://s3.amazonaws.com/sixthviewfs/libs/mkl/linux/silent.cfg 
cp silent.cfg l_mkl_$APPVERSION/
l_mkl_$APPVERSION/install.sh --silent l_mkl_$APPVERSION/silent.cfg
