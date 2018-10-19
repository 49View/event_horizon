var express = require('express');
var router = express.Router();

var dp = require('../controllers/deployController');

router.get('/all', dp.deploy_all );

module.exports = router;