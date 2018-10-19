
var express = require('express');
var router = express.Router();

var mapper = require("../assistants/keywordObjectMapper");
const material = require('../models/material');
const geom = require('../models/geom');

function newMaterial() {
    return new mapper.MapObject(material, "material", ".mat", [ "_hi.mat", "_low.mat", "_med.mat"], true, "white.mat" );
}

function newGeom() {
    return new mapper.MapObject(geom, "geom", ".geom", [], false, "" );
}

router.post('/upload/material/:key', (req, res, next) => {
    mapper.uploadObject(res, req, newMaterial() );
});

router.get('/get/materials/:key', (req, res, next) => {
    mapper.getObject( res, req, newMaterial() );
});

router.get('/map/material/:key', (req, res, next) => {
    mapper.mapObject( req.params.key, res, newMaterial() );
});

router.get('/map/materials', (req, res, next) => {
    mapper.mapObjects( res, newMaterial() );
});

router.post('/upload/geoms/:key', (req, res, next) => {
    mapper.uploadObject(res, req, newGeom() );
});

router.get('/get/geoms/:key', (req, res, next) => {
    mapper.getObject( res, req, newGeom() );
});

router.get('/map/geom/:key', (req, res, next) => {
    mapper.mapObject( req.params.key, res, newGeom() );
});

module.exports = router;
