#
# NameCheap point domains to digital ocean
#
# Digital Ocean: 
# 
#   - Add a domain
#   - Create droplet
#   - Create floating IP
#   - Set A name and CMANAE

sudo ufw allow 80
sudo ufw allow 443

sudo mkdir -p /docker/letsencrypt-docker-nginx/src/letsencrypt/letsencrypt-site

echo -e "version: '3.1'

services:

  letsencrypt-nginx-container:
    container_name: 'letsencrypt-nginx-container'
    image: nginx:latest
    ports:
      - "80:80"
    volumes:
      - ./nginx.conf:/etc/nginx/conf.d/default.conf
      - ./letsencrypt-site:/usr/share/nginx/html
    networks:
      - docker-network

networks:
  docker-network:
    driver: bridge" | sudo tee /docker/letsencrypt-docker-nginx/src/letsencrypt/docker-compose.yml;

echo -e "server {
    listen 80;
    listen [::]:80;
    server_name doublecanny.com www.$1;

    location ~ /.well-known/acme-challenge {
        allow all;
        root /usr/share/nginx/html;
    }

    root /usr/share/nginx/html;
    index index.html;
}" | sudo tee /docker/letsencrypt-docker-nginx/src/letsencrypt/nginx.conf

echo -e "
<html>
<head>
    <meta charset='utf-8' />
    <title>Let's Encrypt First Time Cert Issue Site</title>
</head>
<body>
    <h1>Oh, hai there!</h1>
    <p>
        This is the temporary site that will only be used for the very first time SSL certificates are issued by Let's Encrypt's
        certbot.
    </p>
</body>
</html>" | sudo tee /docker/letsencrypt-docker-nginx/src/letsencrypt/letsencrypt-site/index.html;

cd /docker/letsencrypt-docker-nginx/src/letsencrypt
sudo docker-compose up -d

sudo docker run -it --rm \
-v /docker-volumes/etc/letsencrypt:/etc/letsencrypt \
-v /docker-volumes/var/lib/letsencrypt:/var/lib/letsencrypt \
-v /docker/letsencrypt-docker-nginx/src/letsencrypt/letsencrypt-site:/data/letsencrypt \
-v "/docker-volumes/var/log/letsencrypt:/var/log/letsencrypt" \
certbot/certbot \
certonly --webroot \
--register-unsafely-without-email --agree-tos \
--webroot-path=/data/letsencrypt \
--staging \
-d $1 -d www.$1

sudo rm -rf /docker-volumes/

sudo docker run -it --rm \
-v /docker-volumes/etc/letsencrypt:/etc/letsencrypt \
-v /docker-volumes/var/lib/letsencrypt:/var/lib/letsencrypt \
-v /docker/letsencrypt-docker-nginx/src/letsencrypt/letsencrypt-site:/data/letsencrypt \
-v "/docker-volumes/var/log/letsencrypt:/var/log/letsencrypt" \
certbot/certbot \
certonly --webroot \
--email messingaroundbigtime@gmail.com --agree-tos --no-eff-email \
--webroot-path=/data/letsencrypt \
-d $1 -d www.$1

# sudo openssl dhparam -out ~/dhparam-2048.pem 2048

sudo mkdir /sslcerts
sudo cp /docker-volumes/etc/letsencrypt/live/$1/privkey.pem /sslcerts/
sudo cp /docker-volumes/etc/letsencrypt/live/$1/fullchain.pem /sslcerts/
