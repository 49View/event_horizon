const winston = require("winston");

const { createLogger, format, transports } = require("winston");
const appRoot = require("app-root-path");

const defFormat = (process.env.NODE_ENV !== 'production') ? winston.format.combine(
    winston.format.colorize(),
    winston.format.simple()
) : winston.format.json();
const defTransport = (process.env.NODE_ENV !== 'production') ? new transports.Console() :  new transports.File({ filename: `${appRoot}/logs/nodejs-api.log` }) ;

const logger = createLogger({
  level: "silly",
  exitOnError: false,
  format: defFormat,
  transports: [
    defTransport
  ]
});

module.exports = logger;
