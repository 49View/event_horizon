const mongoose = require('mongoose');
const express = require('express');
const logger = require('morgan');
const bodyParser = require('body-parser');
const cookieParser = require('cookie-parser');
const passport = require('passport');
const globalConfig = require('./config_api.js')
const indexRoute = require('./routes/indexRoute');
const entitiesRoute = require('./routes/entitiesRoute');
const usersRoute = require('./routes/usersRoute');
const tokenRoute = require('./routes/tokenRoute');
const fsRoute = require('./routes/fsRoute');
const authController = require('./controllers/authController');
const cryptoController = require('./controllers/cryptoController');

const app = express();


express.static.mime.types["wasm"] = "application/wasm";

//Set up default mongoose connection
const mongoDB = `mongodb+srv://${globalConfig.MongoDBUser}:${globalConfig.MongoDBPass}@${globalConfig.MongoDBURI}`;
mongoose.connect(mongoDB, { dbName: globalConfig.MongoDBdbName, useNewUrlParser: true });

//Get the default connection
let db = mongoose.connection;

//Bind connection to error event (to get notification of connection errors)
db.on('error', console.error.bind(console, 'MongoDB connection error:'));


// app.use(cookieParser(globalConfig.mJWTSecret));

// app.use( (req,res,next) => {
//   if (req.method === 'OPTIONS') {
//     console.log('!OPTIONS');
//     var headers = {};
//     // IE8 does not allow domains to be specified, just the *
//     headers["Access-Control-Allow-Origin"] = req.headers.origin;
//     // headers["Access-Control-Allow-Origin"] = "*";
//     headers["Access-Control-Allow-Methods"] = "POST, GET, PUT, DELETE, OPTIONS";
//     headers["Access-Control-Allow-Credentials"] = true;
//     headers["Access-Control-Max-Age"] = '86400'; // 24 hours
//     headers["Access-Control-Allow-Headers"] = "X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept, Cache-Control";
//     res.writeHead(200, headers);
//     res.end();
//   } else {
//     next();
//   }  
// });

// // app.use((req, res, next) => {
// //   res.header("Access-Control-Allow-Origin", "*");
// //   res.header('Access-Control-Allow-Credentials', true);
// //   res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
// //   res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Cache-Control");
// //   next();
// // });

// app.use(logger('dev'));
// app.use(bodyParser.raw({limit: '100mb'}));
// app.use(bodyParser.json({limit: '100mb'}));
// app.use(bodyParser.urlencoded({limit: '100mb', extended: true }));

// app.use(cryptoController.decodeRequest);
// app.use(cryptoController.checkRequest);


authController.InitializeAuthentication();

 
app.use(logger('dev'));
app.use(bodyParser.raw({limit: '100mb'}));
app.use(bodyParser.json({limit: '100mb'}));
app.use(bodyParser.urlencoded({limit: '100mb', extended: true }));
app.use(cookieParser(globalConfig.mJWTSecret));

app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Cache-Control, Set-Cookie");
  if (req.method === 'OPTIONS') {
    console.log("REQUEST OPTIONS");
    console.log("URL:", req.url);
    console.log("HEADERS:", req.headers);
    res.status(200).send();
  } else {
    console.log("OTHER REQUEST");
    console.log("METHOD:", req.method);
    console.log("URL:", req.url);
    console.log("HEADERS:", req.headers);
    console.log("COOKIES:", req.cookies);
    console.log("SIGNED-COOKIES:", req.signedCookies);
    next();
  }
 });

 
app.use('/', indexRoute);
app.use('/', tokenRoute);

app.use(passport.authenticate(['client-cert','jwt'], {session:false}));
// app.use(authController.authorize);

app.use('/user', usersRoute);
app.use('/fs', fsRoute);
app.use('/entities', entitiesRoute);


// catch 404 and forward to error handler
app.use((req, res, next) => {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use((err, req, res, next) => {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
