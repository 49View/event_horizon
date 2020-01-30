docker build -t ateventhorizon/49view-portal ./portal
docker build -t ateventhorizon/49view-restful-api ./restful-api
docker build -t ateventhorizon/49view-nginx ./nginx
docker build -t ateventhorizon/49view-mongo-rsinitiate ./mongo

# echo $ateventhorizondockerpass | docker login -u "$ATEVENTHORIZON_DOCKER_ID" --password-stdin
