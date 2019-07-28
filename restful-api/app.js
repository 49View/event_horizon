const mongoose = require("mongoose");
const express = require("express");
const logger = require("morgan");
const bodyParser = require("body-parser");
const cookieParser = require("cookie-parser");
const passport = require("passport");
const globalConfig = require("./config_api.js");
const indexRoute = require("./routes/indexRoute");
const entitiesRoute = require("./routes/entitiesRoute");
const broadcastRoute = require("./routes/broadcastRoute");
const usersRoute = require("./routes/usersRoute");
const tokenRoute = require("./routes/tokenRoute");
const stripeRoute = require("./routes/stripeRoute");
const fsRoute = require("./routes/fsRoute");
const socketRoute = require("./routes/socketRoute");
const authController = require("./controllers/authController");
const projectController = require("./controllers/projectController");
const cryptoController = require("./controllers/cryptoController");

const app = express();

express.static.mime.types["wasm"] = "application/wasm";

//Set up default mongoose connection
const mongoDB = `mongodb+srv://${globalConfig.MongoDBUser}:${
  globalConfig.MongoDBPass
}@${globalConfig.MongoDBURI}`;
mongoose.connect(mongoDB, {
  dbName: globalConfig.MongoDBdbName,
  useNewUrlParser: true
});

//Get the default connection
let db = mongoose.connection;

//Bind connection to error event (to get notification of connection errors)
db.on("error", console.error.bind(console, "MongoDB connection error:"));

//cryptoController.generateKey();
authController.InitializeAuthentication();

app.use(logger("dev"));
app.use(bodyParser.raw({ limit: "100mb" }));
app.use(bodyParser.json({ limit: "100mb" }));
app.use(bodyParser.urlencoded({ limit: "100mb", extended: true }));
app.use(cookieParser(globalConfig.mJWTSecret));

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", req.headers.origin);
  res.header("Access-Control-Allow-Credentials", "true");
  res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
  res.header("Access-Control-Expose-Headers", "ETag");
  res.header("Access-Control-Expose-Headers", "ETag");
  res.header(
    "Access-Control-Allow-Headers",
    "access-control-allow-origin, Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers, Accept, Cache-Control, Set-Cookie, x-eventhorizon-guest, ETag, Authorization"
  );
  if (req.method === "OPTIONS") {
    res.status(200).send();
  } else {
    next();
  }
});

app.use("/", indexRoute);
app.use("/", tokenRoute);
app.use("/stripe", stripeRoute);

app.use(authController.authenticate);
app.use(projectController.checkProjectRoutes);

app.use("/user", usersRoute);
app.use("/fs", fsRoute);
app.use("/entities", entitiesRoute);
app.use("/broadcast", broadcastRoute);
app.use("/socket", socketRoute);

// catch 404 and forward to error handler
app.use((req, res, next) => {
  var err = new Error("Not Found");
  err.status = 404;
  next(err);
});

// error handler
app.use((err, req, res, next) => {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get("env") === "development" ? err : {};

  // render the error page
  res.status(err.status || 500).send();
});

module.exports = app;
