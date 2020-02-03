install_boost.sh 1 72 0

gitbuild.sh glm 0.9.9.6 g-truc
gitbuild.sh stb 0.0.1 ziocleto
gitbuild.sh rapidjson v1.1.1 ziocleto "-DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF"
gitbuild.sh restbed 4.7.0 ziocleto "-DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_LIBDIR=lib"
gitbuild.sh libzip rel-1-5-2 nih-at "-DBUILD_SHARED_LIBS=FALSE -DENABLE_COMMONCRYPTO=FALSE -DBUILD_TOOLS=FALSE -DBUILD_REGRESS=FALSE -DBUILD_EXAMPLES=FALSE -DBUILD_DOC=FALSE"
gitbuild.sh websocketpp 0.8.2 ziocleto
gitbuild.sh mongo-c-driver 1.15.3 mongodb "-DCMAKE_INSTALL_PREFIX=/usr/local"
gitbuild.sh mongo-cxx-driver r3.4.0 mongodb "-DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_BOOST=1 -DBUILD_SHARED_LIBS=OFF"

gitbuild.sh tinygltf master syoyo "-DTINYGLTF_BUILD_EXAMPLES=FALSE -DCMAKE_INSTALL_PREFIX=/usr/local/include/tinygltf"
gitbuild.sh tinydngloader master syoyo "-DTINYGLTF_BUILD_EXAMPLES=FALSE -DCMAKE_INSTALL_PREFIX=/usr/local/include/tinygltf"


./pom.sh boost 1 72 0

./pom.sh mongo-c-driver 1.15.3 mongodb
./pom.sh mongo-cxx-driver r3.4.0 mongodb "-DBSONCXX_POLY_USE_BOOST=1"