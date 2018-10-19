var floorplanCalculusModel = require('../models/floorplan_calculus');
var cloudApi = require('../third_party_api/s3');

var server = require('../bin/www');

exports.create_post = ( req, res ) => {
	const key = req.params.key;
	var p1 = cloudApi.upload( req.body, "images/floorplans/" + key );
	p1.then( data => {		
		const json = { 
			msg : 'floorplan2dto3d',
			name : key 
		};
		let fps = new floorplanCalculusModel(json);
   	    fps.save().then( () => {
	     	server.sio.emit('message', JSON.stringify(json) );
	     	res.json(json);
	    }).catch( err => {
  	    	console.log(err, err.stack);
   			res.status(500).send( "[DB ERROR] Table: floorplan_calculus " + JSON.stringify(json) + " failed to save" );
	    });
	}).catch( err => {
  	    console.log(err, err.stack);
   		res.status(500).send(`File ${req.params.key} could not be uploaded`)
	});
}
