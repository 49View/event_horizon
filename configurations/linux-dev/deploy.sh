sudo rm -rf event_horizon
sudo git clone git@github.com:49View/event_horizon.git

cp /etc/letsencrypt/live/eventhorizon.pw/fullchain.pem ./event_horizon/node-api-server/bin/server-crt.pem
cp /etc/letsencrypt/live/eventhorizon.pw/privkey.pem ./event_horizon/node-api-server/bin/server-key.pem

sudo docker kill $(docker ps --filter "ancestor=49view/node-api-server" -q)
sudo docker build -t eventhorizon-api ./event_horizon/node-api-server
sudo docker run -p 443:3000 -d eventhorizon-api

sudo docker kill $(docker ps --filter "ancestor=49view/node-api-server" -q)
sudo docker build -t eventhorizon-editor ./event_horizon/devtools/editor
sudo docker run -p 8000:8000 -d eventhorizon-editor

