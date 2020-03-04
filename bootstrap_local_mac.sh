pushd
cd /usr/local/etc && sudo nginx
popd
mongod --port 27017 --replSet rs0 --dbpath ~/Documents/mongodata
