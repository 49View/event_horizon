FROM node:13.7.0 as builder
ARG REACT_APP_EH_CLOUD_HOST_ARG
ENV REACT_APP_EH_CLOUD_HOST=$REACT_APP_EH_CLOUD_HOST_ARG
WORKDIR /app
COPY ./package.json ./
RUN npm install
COPY . .
RUN npm run build

FROM nginx
EXPOSE 3000
COPY ./nginx-portal/default.conf /etc/nginx/conf.d/default.conf
COPY --from=builder /app/build /usr/share/nginx/html
