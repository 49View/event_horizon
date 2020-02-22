gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'sudo systemctl restart nginx && tail -f /var/log/nginx/access.log'

gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'sudo mongod --port 27017 --replSet rs0'

gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'cd ~/Documents/GitHub/arch/event_horizon/restful-api && nodemon --max-old-space-size=16000 ./bin/www'