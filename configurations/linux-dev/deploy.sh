sudo rm -rf event_horizon
sudo git clone git@github.com:49View/event_horizon.git

#cp ./server-crt.pem ./event_horizon/node-api-server/bin/server-crt.pem
#cp ./server-key.pem ./event_horizon/node-api-server/bin/server-key.pem

sudo systemctl stop nginx

sudo docker kill $(docker ps --filter "ancestor=eventhorizon-api" -q)
sudo docker build -t eventhorizon-api ./event_horizon/node-api-server
sudo docker run -p:3000:3000 -d eventhorizon-api

sudo docker kill $(docker ps --filter "ancestor=eventhorizon-editor" -q)
sudo docker build -t eventhorizon-editor ./event_horizon/dev_tools/editor
sudo docker run -p:4000:4000 -d eventhorizon-editor

sudo systemctl start nginx

#user token:
1a169b000383285269569bfbb320db0e6b6e90ca
