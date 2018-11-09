//var db = require('../app');

var express = require('express');
var router = express.Router();
var sharp = require('sharp');

var server = require('../bin/www');

var fsc = require('../controllers/fsController');

function writeFile( res, data ) {
    res.writeHead(200, {
        'Content-Type': data.ContentType,
        'Content-Last-Modified': data.LastModified,
        'ETag': data.ETag,
        'Content-Length': data.ContentLength
    });
    res.end(data["Body"]);
}

function writeError( res, number, err, message ) {
    console.log("[ERROR-CAUGHT]");
    console.log(err, err.stack);
    res.status(number).send(message);
}

router.get('/get/:key', (req, res, next) => {

    const p = fsc.cloudStorageFileGet(req.params.key);
    
    p.then( (data) => {
        writeFile( res, data );
    }).catch( (err) => {
        const pkeys = fsc.cloudStorageFileList(req.params.key, true);
        pkeys.then( (data) => {
            if ( !Array.isArray(data) || !data.length ) {
                writeError( res, 404, err, `File ${req.params.key} not found`);
            } else {
                const p2 = fsc.cloudStorageFileGet(data[0]);
                p2.then( data => {
                    writeFile( res, data );
                }, (err) => {
                    writeError( res, 404, err, `File ${req.params.key} not found`);
                });    
            }
        }, (err) => {
            writeError( res, 404, err, `File ${req.params.key} not found`);
        });
    });
});

router.get('/getResize/:w/:h/:key', (req, res, next) => {

    const p = fsc.cloudStorageFileGet(req.params.key);
    
    p.then( (data) => {

        console.log(req.params.w+"x"+req.params.h);
        
        sharp(data["Body"])
            .resize(Number(req.params.w),Number(req.params.h))
            .max()
            .toFormat('png')
            .toBuffer()
            .then(function(outputBuffer) {
                // outputBuffer contains JPEG image data no wider than 200 pixels and no higher
                // than 200 pixels regardless of the inputBuffer image dimensions
                res.writeHead(200, {
                    'Content-Type': 'image/png',
                    'Content-Last-Modified': data.LastModified,
                });
                res.end(outputBuffer);
            });
        /*
        res.writeHead(200, {
            'Content-Type': data.ContentType,
            'Content-Last-Modified': data.LastModified,
            'ETag': data.ETag,
            'Content-Length': data.ContentLength
        });
        */
    }).catch( (err) => {
        console.log(err, err.stack);
        res.status(500).send(`File ${req.params.key} not found`);
    });
});

router.post('/upload/:key', (req, res, next) => {

    const p = fsc.cloudStorageFileUpdate(req.body, req.params.key);

    p.then( (data) => {
		console.log("File uploaded" + data.ETag);
		let json = { msg : 'cloudStorageFileUpdate', name : req.params.key };
		let jsonParticular = { msg : 'cloudStorageFileUpdate-'+req.params.key };
		server.sio.emit( 'message', JSON.stringify(json) );
		server.sio.emit( 'message', JSON.stringify(jsonParticular) );
    	if ( res ) {
			res.writeHead(200, { 'ETag': data.ETag } );
			res.end();		  	    		
    	}		
	}).catch( (err) => {
  	    console.log(err, err.stack);
   		res.status(500).send(`File ${req.params.key} could not be uploaded`)
	});  	    

});

router.get('/remove/:key', function (req, res, next) {
    res.send("NOT implemented yet");
});

router.get('/rename/:source/:dest', function (req, res, next) {
    const p = fsc.cloudStorageFileList( req.params.key, res );

    p.then( (p_res) => {
        res.send(p_res);
   }, (err) => {
       res.status(500).send(`cloudStorageFileList 500 on ${key}`);
   });
});

router.get('/list/:key', (req, res, next) => {
    const p = fsc.cloudStorageFileList( req.params.key, false );

    p.then( (p_res) => {
        res.send(p_res);
   }, (err) => {
       res.status(500).send(`cloudStorageFileList 500 on ${key}`);
   });

});

module.exports = router;
