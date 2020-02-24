gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'sudo systemctl restart nginx && tail -f /var/log/nginx/access.log'

gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'export EH_MONGO_DEFAULT_DB=$1 && sudo mongod --port 27017 --replSet rs0'

gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'cd ./restful-api && nodemon --max-old-space-size=16000 ./bin/www'