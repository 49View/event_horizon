var fsc = require('../controllers/fsController');
var server = require('../bin/www');
var express = require('express');
var router = express.Router();

router.get('/:key', async (req, res, next) => {

    try {
        const data = await fsc.cloudStorageFileGet(req.params.key, "eventhorizonfs");
        fsc.writeFile(res, data);
    } catch (ex) {
        console.log("ERROR GETTING FILE FROM FS: ",ex);
        res.sendStatus(400);
    }
});

router.post('/:key', async (req, res, next) => {

    try {
        const data = await fsc.cloudStorageFileUpload(req.body, req.params.key, "eventhorizonfs");
		let json = { msg : 'cloudStorageFileUpdate', name : req.params.key };
		let jsonParticular = { msg : 'cloudStorageFileUpdate-'+req.params.key };
		server.ws_send( JSON.stringify(json) );
		server.ws_send( JSON.stringify(jsonParticular) );
    	if ( res ) {
			res.status(201).json({ 'ETag': data.ETag });
			res.end();		  	    		
    	}		
        
    } catch (ex) {
        console.log("ERROR ADDING FILE TO FS: ", ex);
        res.sendStatus(400);
    }
});

router.delete('/:key', async (req, res, next) => {
    
    try {
        const result = await fsc.cloudStorageDelete(req.params.key, "eventhorizonfs");
        res.sendStatus(204);
    } catch (ex) {
        console.log("ERROR DELETING FILE IN FS: ", ex);
        res.sendStatus(400);
    }
});

module.exports = router;
