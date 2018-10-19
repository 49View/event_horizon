var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

var index = require('./routes/index');
var catalog = require('./routes/catalog');
var calculus = require('./routes/calculus');
var service = require('./routes/service');
var fs = require('./routes/fs');
var fsmat = require('./routes/fsmat');
var fsmatcol = require('./routes/fsmatcol');
var fsentity = require('./routes/fsentity');
var deploy = require('./routes/deploy');

var app = express();

//Set up default mongoose connection
var mongoose = require('mongoose');
var mongoDB = "mongodb+srv://dado:luckycarrot1803@sixthviewmdb001-a0hba.mongodb.net";
//var mongoDB = 'mongodb://localhost:27017/sixthviewdb';
mongoose.connect(mongoDB, { dbName: 'sixthviewdb', useNewUrlParser: true });

//Get the default connection
let db = mongoose.connection;

//Bind connection to error event (to get notification of connection errors)
db.on('error', console.error.bind(console, 'MongoDB connection error:'));

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');


app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Cache-Control");
  next();
});

// uncomment after placing your favicon in /public
app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
// app.use(bodyParser);
app.use(bodyParser.raw({limit: '100mb'}));
app.use(bodyParser.json({limit: '100mb'}));
app.use(bodyParser.urlencoded({limit: '100mb', extended: true }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', index);
app.use('/catalog', catalog);
app.use('/calculus', calculus);
app.use('/service', service);
app.use('/fs', fsmat);
app.use('/fs', fsmatcol);
app.use('/fs', fsentity);
app.use('/fs', fs);
app.use('/deploy', deploy);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
