sudo rm -rf event_horizon
sudo git clone git@github.com:49View/event_horizon.git

cp ./server-crt.pem ./event_horizon/node-api-server/bin/server-crt.pem
cp ./server-key.pem ./event_horizon/node-api-server/bin/server-key.pem

sudo docker kill $(docker ps --filter "ancestor=eventhorizon-api" -q)
sudo docker build -t eventhorizon-api ./event_horizon/node-api-server
sudo docker run -p 443:3000 -d eventhorizon-api

sudo docker kill $(docker ps --filter "ancestor=eventhorizon-editor" -q)
sudo docker build -t eventhorizon-editor ./event_horizon/dev_tools/editor
sudo docker run -p 8000:8000 -d eventhorizon-editor

