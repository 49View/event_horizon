var express = require('express');
var path = require('path');

var app = express();
app.listen(4000);

app.use(express.static(path.join(__dirname, 'build')));

app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Cache-Control");
    next();
  });
  
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.sendFile(path.join(__dirname, 'build/index.html'));
  // res.send('Node api status: <img src="https://circleci.com/gh/49View/node-api-server.svg?style=svg&circle-token=0312dab6c17d25ac4cde35689efc24eb1616d3aa">');
});