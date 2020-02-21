sudo systemctl restart nginx
tail -f /var/log/nginx/access.log

sudo mongod --port 27017 --replSet rs0

cd restful-api && nodemon --max-old-space-size=16000 ./bin/www