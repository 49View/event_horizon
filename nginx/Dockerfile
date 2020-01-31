FROM nginx
COPY ./http.conf /etc/nginx/conf.d/http.conf
COPY ./stream.conf /etc/nginx/conf.d/stream.conf
COPY ./nginx.conf /etc/nginx/nginx.conf
COPY ./ca.crt /etc/nginx/client_certs/ca.crt
COPY ./dhparam-2048.pem /etc/ssl/certs/dhparam-2048.pem
