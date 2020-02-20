sudo apt-get update
sudo apt-get install -y libssl-dev apt-transport-https libglew-dev libglfw3 libglfw3-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev libopenal-dev libstdc++-9-dev libbz2-dev libjpeg-dev libpng-dev libtiff-dev libwebp-dev nettle-dev gnutls-dev libicu-dev npm 

pom-get.sh openssl 			    /usr/local/
pom-get.sh mongo-c-driver   /usr/local/
pom-get.sh mongo-cxx-driver /usr/local/
pom-get.sh libzip           /usr/local/
pom-get.sh restbed          /usr/local/
pom-get.sh rapidjson        /usr/local/

pom-get.sh openjpeg 		    /usr/local/
pom-get.sh leptonica        /usr/local/
pom-get.sh tesseract        /usr/local/

pom-get.sh stb              /usr/local/
pom-get.sh websocketpp      /usr/local/
pom-get.sh boost            /usr/local/
pom-get.sh opt-sat-indie    /opt/Allegorithmic/
pom-get.sh fbx2gltf-linux   /usr/local/bin/
pom-get.sh tinygltf         /usr/local/include/tinygltf/
pom-get.sh opencv           /usr/local/
pom-get.sh glm 				      /usr/local/

sudo gitbuild-make.sh openssl OpenSSL_1_1_1d openssl "-DBUILD_TOOLS=FALSE -DBUILD_EXAMPLES=FALSE" /usr/local
sudo gitbuild.sh mongo-c-driver 1.15.3 mongodb "-DBUILD_TOOLS=FALSE -DBUILD_EXAMPLES=FALSE" /usr/local
sudo gitbuild.sh mongo-cxx-driver r3.4.0 mongodb "" /usr/local
sudo gitbuild.sh libzip rel-1-5-2 nih-at "-DBUILD_TOOLS=FALSE -DBUILD_EXAMPLES=FALSE -DBUILD_REGRESS=false -DBUILD_DOC=FALSE -DENABLE_GNUTLS=OFF" /usr/local
sudo gitbuild.sh restbed 4.7.0 ziocleto "-DBUILD_TOOLS=FALSE -DBUILD_EXAMPLES=FALSE" /usr/local

sudo gitbuild.sh rapidjson v1.1.1 ziocleto "" /usr/local

sudo gitbuild.sh openjpeg v2.3.1 uclouvain "" /usr/local
sudo gitbuild.sh leptonica 1.79.0 DanBloomberg "" /usr/local
sudo gitbuild.sh tesseract 4.1.1 tesseract-ocr "-DSTATIC=TRUE" /usr/local

# Install dev environment

# install node.js
curl -sL https://deb.nodesource.com/setup_13.x | sudo -E bash -
sudo apt-get install -y nodejs
sudo npm install -g nodemon

sudo apt install nginx

# MongoDB
wget -qO - https://www.mongodb.org/static/pgp/server-4.2.asc | sudo apt-key add -
echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu bionic/mongodb-org/4.2 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-4.2.list
sudo apt-get update
sudo apt-get install -y mongodb-org
sudo mkdir /data
sudo mkdir /data/db 

# Mkcert to create localhost SSL certs
apt install libnss3-tools -y
wget https://github.com/FiloSottile/mkcert/releases/download/v1.1.2/mkcert-v1.1.2-linux-amd64
mv mkcert-v1.1.2-linux-amd64 mkcert
chmod +x mkcert
sudo cp mkcert /usr/local/bin/
mkcert -install
sudo mkcert example.com '*.example.com' localhost 127.0.0.1 ::1
sudo mkdir /sslcerts
sudo mv ./example.com+4.pem /sslcerts/dns-cert.pem
sudo mv ./example.com+4-key.pem /sslcerts/dns-cert-key.pem

# Startup the system
mongod --port 27017 --replSet rs0
sudo systemctl start nginx

