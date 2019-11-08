const { createLogger, format, transports } = require("winston");
var appRoot = require("app-root-path");

const logger = createLogger({
  level: "info",
  exitOnError: false,
  format: format.json(),
  transports: [
    new transports.File({ filename: `${appRoot}/logs/nodejs-api.log` })
  ]
});

module.exports = logger;
