sudo docker kill $(docker ps -q)
sudo docker rm $(docker ps -a -q)
sudo docker rmi $(docker images -q)

sudo rm -rf event_horizon
sudo git clone git@github.com:49View/event_horizon.git

sudo docker build -t 49view/node-api-server ./event_horizon/node-api-server

sudo systemctl restart nginx

sudo docker run -p 3000:3000 -d 49view/node-api-server
