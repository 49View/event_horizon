sudo mkdir sat
cd sat

echo -e "FROM "$1"\nADD substance_daemon /\nCMD [\"./substance_daemon\" ]" | tee dockerfile

curl https://eventhorizon.pw/fs/get/artifacts%2Fsubstance_daemon -o substance_daemon
sudo chmod 777 substance_daemon 

sudo docker kill $(docker ps --filter "ancestor=sat" -q)

sudo docker build -t sat .

sudo docker run -d sat
