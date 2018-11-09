var server = require('../bin/www');
var fsc = require('../controllers/fsController');

exports.deploy_all = ( req, res ) => {
    let p = fsc.cloudStorageRename( "wasm/builds/sandbox.js", "wasm/sandbox.js" );
    p.then( (data) => {
		console.log("Deployed native em sandbox.js");
        let p1 = fsc.cloudStorageRename( "wasm/builds/sandbox.wasm", "wasm/sandbox.wasm" );
        p1.then( (data) => {
            console.log("Deployed native em sandbox.wasm");
            let json = { "result" : "ok" };
            if ( res ) {
                res.json( json );
            }		
        }).catch( (err) => {
            console.log(err, err.stack);
             res.status(500).send(`File sandbox.wasm has some problem`)
      })
	}).catch( (err) => {
  	    console.log(err, err.stack);
   		res.status(500).send(`File sandbox.js has some problem`)
	});  	
}
