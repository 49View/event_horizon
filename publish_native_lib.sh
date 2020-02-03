echo $3 | docker login -u $1 --password-stdin
docker push $1/lib$2