var express = require('express');
var router = express.Router();

var sc = require('../controllers/serviceController');
var ssc = require('../controllers/socketController');

router.post('/floorplanImporter/url', sc.create_post );
router.get('/ping', ssc.ping );
router.post('/send', ssc.send );

module.exports = router;