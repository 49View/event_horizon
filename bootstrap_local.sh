gnome-terminal --window-with-profile=eh_bootstrap -- bash -c 'systemctl restart nginx && tail -f /var/log/nginx/access.log'

gnome-terminal --window-with-profile=eh_bootstrap -- /bin/bash -c 'mongod --port 27017 --replSet rs0'
