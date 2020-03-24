'use strict';
const express = require("express");
const bodyParser = require("body-parser");
const cookieParser = require("cookie-parser");
const globalConfig = require("./config_api.js");
const indexRoute = require("./routes/indexRoute");
const usersRoute = require("./routes/usersRoute");
const tokenRoute = require("./routes/tokenRoute");
const authController = require("./controllers/authController");
const db = require("./db");

const app = express();

express.static.mime.types["wasm"] = "application/wasm";

console.log("Started");

//Set up default mongoose connection
db.initDB();

app.use(bodyParser.raw({limit: "500mb", type:'application/octet-stream'}));
app.use(bodyParser.text({limit: "500mb"}));
app.use(bodyParser.json({limit: "100mb"}));
app.use(bodyParser.urlencoded({limit: "100mb", extended: true}));
app.use(cookieParser(globalConfig.mJWTSecret));

authController.initializeAuthentication();

app.use(function (req, res, next) {
    // console.log( req.headers );
    res.header("Access-Control-Allow-Origin", req.headers.origin);
    res.header("Access-Control-Allow-Credentials", "true");
    res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
    res.header("Access-Control-Expose-Headers", "ETag");
    res.header("Access-Control-Expose-Headers", "ETag");
    res.header(
        "Access-Control-Allow-Headers",
        "access-control-allow-origin, Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers, Accept, Cache-Control, Set-Cookie, x-eventhorizon-guest, x-eventhorizon-guest-write, ETag, Authorization"
    );
    if (req.method === "OPTIONS") {
        res.status(200).send();
    } else {
        next();
    }
});

app.use("/", indexRoute);
app.use("/", tokenRoute);
app.use("/user", usersRoute);

app.use(authController.authenticate);


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
