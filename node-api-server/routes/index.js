var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.send('Node api status: <img src="https://circleci.com/gh/49View/node-api-server.svg?style=svg&circle-token=0312dab6c17d25ac4cde35689efc24eb1616d3aa">');
});

module.exports = router;
