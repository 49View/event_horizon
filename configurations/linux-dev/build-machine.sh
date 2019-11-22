apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y ibglu1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxext-dev libxi-dev libxcursor-dev libxinerama-dev mesa-utils tesseract-ocr tesseract-ocr-eng libtesseract-dev libleptonica-dev cpio && apt-get clean && rm -rf /var/lib/apt/lists/*

./install_cmake.sh 3 12 0
./install_git-lfs.sh 2.4.1

# Libraries in order of dependencies
./install_boost.sh 1 67 0

# We do NOT install embree and mkl for now as all SH lighting tech has been postponed
# ./install_embree.sh 3.2.0
# ./install_mkl.sh 2018.2.199

if [ ! "$OSNAME" == "linux" ]; then
./gitbuild_openssl.sh openssl OpenSSL_1_1_0h openssl darwin64-x86_64-cc
fi

./gitbuild.sh poshlib 0.0.1 ziocleto
./gitbuild.sh thekla_atlas 0.0.5 ziocleto

./gitmakebuild.sh asio asio-1-12-0b ziocleto
./gitbuild.sh kashmir-dependency 0.0.3 ziocleto
./gitbuild.sh catch-dependency 0.0.1 ziocleto
./gitbuild.sh rapidjson v1.1.0 Tencent "-DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF"
./gitbuild.sh websocketpp 0.7.1 ziocleto

./gitbuild.sh curlpp v0.8.4 ziocleto
./gitbuild.sh socket.io-client-cpp 1.6.1.2 ziocleto 
./gitbuild.sh restbed 4.6.5 ziocleto "-DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_LIBDIR=lib"

# opencv 
./gitbuild.sh opencv 3.4.1 opencv "-DBUILD_SHARED_LIBS=FALSE -DBUILD_TESTS=FALSE -DBUILD_OPENEXR=TRUE -DBUILD_WEBP=TRUE -DHAVE_DSHOW=FALSE -DBUILD_JAVA=FALSE -DBUILD_opencv_java=FALSE -DBUILDopencv_java_bindings_generator=FALSE" 
./postbuild_opencv.sh

./gitbuild.sh glfw 3.2.1 glfw "-DBUILD_SHARED_LIBS=FALSE" 
./gitbuild_glew.sh glew glew-2.1.0 nigels-com "-DBUILD_SHARED_LIBS=FALSE" 

OPENVR_VERSION=1.0.15
./gitbuild.sh openvr v$OPENVR_VERSION ValveSoftware "-DBUILD_SHARED_LIBS=FALSE" 
./postbuild_openvr.sh $OPENVR_VERSION

# misc
./install_cimg.sh
./gitbuild.sh stb 0.0.1 ziocleto
./gitbuild.sh efsw 0.0.1 ziocleto "-DSTATIC_LIB=TRUE"

# emscripten
./install_emscripten.sh

if [ ! "$OSNAME" == "windows" ]; then
./gitbuild.sh leptonica 1.76.0 DanBloomberg "-DJPEG_INCLUDE_DIR=C:\usr\local\include\libjpeg -DTIFF_INCLUDE_DIR=C:\usr\local\include\libtiff -DZLIB_INCLUDE_DIR=C:\usr\local\include\zlib -DSTATIC=TRUE"
./gitbuild.sh tesseract 4.0.0-rc1d2 ziocleto "-DSTATIC=TRUE -DLeptonica_DIR=c:\usr\local\cmake"
fi
