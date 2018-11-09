var express = require('express');
var router = express.Router();

var cc = require('../controllers/calculusController');

router.post('/fp2d3d/:key', cc.create_post );

module.exports = router;