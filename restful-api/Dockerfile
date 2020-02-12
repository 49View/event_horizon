FROM node:13.7.0
WORKDIR /app
COPY ./package.json ./
RUN npm install
COPY . .
EXPOSE 4000
EXPOSE 3500
CMD [ "npm", "run", "start" ]
