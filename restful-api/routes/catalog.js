var express = require('express');
var router = express.Router();

var House = require('../models/house');
var OsmArea = require('../models/osm_area')
var StyleSheet = require('../models/ui_stylesheet');
var Layout = require('../models/ui_layout');
var PropertyListing = require('../models/property_listing');
var UIView = require('../models/ui_view');
var MLMoments = require('../models/ml_moments');
var AppOptions = require('../models/app_options');
var Calculus = require('../models/calculus');
var CalculusTask = require('../models/calculus_task');
var Material = require('../models/material');
var MaterialColor = require('../models/material_color');

// Require controller modules
var cu = require('../controllers/controllerUtils');
var fs = require('../controllers/fsController');
var house_controller = require('../controllers/houseController');

// Composite functions
function deleteHouse( req, res ) {
    PropertyListing.findByIdAndDelete( req.params.id, (error, result) => {
        if (error) { 
			console.log(error); 
			res.send({result:null, error: error});
		} else {
            House.findOneAndDelete( { name: req.params.id }, (error, result) => {
                if (error) { 
                    console.log(error);
                    res.send({result:null, error: error}); 		
                } else {
					const p = fs.cloudStorageDeleteMulti( "images/properties/" + req.params.id );
					p.then( (data) => {
						
					}).catch( (err) => {
                    	console.log(err, err.stack);
                	});   
                    res.send({ result: req.params.id, error: null});
                }
            });
		}
    });
}

function deleteHouses( req, res ) {
    PropertyListing.find( {}, (error, result) => {
		if ( result && result.length > 0 ) {
			const idList = [];
			result.forEach( elem => {
				idList.push( elem._id );
			});
			idList.forEach( value => {
				PropertyListing.findByIdAndDelete( value, (error, result) => {
					if (error) { 
						console.log(error); 
						res.send({result:null, error: error});
						return;
					} else {
						House.findOneAndDelete( { name: value }, (error, result) => {
							if (error) { 
								console.log(error);
								res.send({result:null, error: error}); 		
								return;
							} else {
								const p = fs.cloudStorageDeleteMulti( "images/properties/" + value );
								p.then( (data) => {									
								}).catch( (err) => {
									console.log(err, err.stack);
									res.send({ result: value, error: null});
									return;
								});   
							}
						});
					}
				});		
			});
		}
		res.send({ result: 'deleteHouses', error: null});
	});
}

var addRouterFor = function(path,model,types) {
	var basepath;
	
	basePath="/"+path+"/";
	if(types.indexOf("CR")>=0) {
		router.post(basePath+'create/', function(req, res) { cu.create_post( model, req, res ); } );		
	}
	if(types.indexOf("CU")>=0) {
		router.post(basePath+'create_and_upload', (req, res) => { cu.create_and_upload_post( model, req, res ); } );		
	}
	if(types.indexOf("IA")>=0) {
		router.post(basePath+'insert_array/', function(req, res) { cu.insert_array_post( model, req, res ); } );
	}
	if(types.indexOf("CK")>=0) {
		router.post(basePath+'create/:key', function(req, res) { cu.create_post_with_key( model, req, res ); } );		
	}
	if(types.indexOf("FC")>=0) {
		router.post(basePath+'get_complex/', function(req, res) { cu.findComplex( model, req, res ); });	
	}
	if(types.indexOf("NE")>=0) {
		router.get(basePath+'get_notexact/:key/', function(req, res) { cu.findOneNotExact( model, req, res ); });	
	}
	if(types.indexOf("GK")>=0) {
		router.get(basePath+'get/:key/:id', function(req, res) { cu.findOne( model, req, res ); });	
	}
	if(types.indexOf("GM")>=0) {
		router.get(basePath+'get_many/:key/:id', function(req, res) { cu.findMany( model, req, res ); });
	}
	if(types.indexOf("GD")>=0) {
		router.get(basePath+'get_distinct/:id', function(req, res) { cu.distinct( model, req, res ); });
	}
	if(types.indexOf("UK")>=0) {
		router.post(basePath+'update/', function(req, res) { cu.update_with_query( model, req, res ); });	
	}
	if(types.indexOf("SM")>=0) {
		router.post(basePath+'poll_request/', function(req, res) { cu.update_with_query( model, req, res ); });	
	}
	if(types.indexOf("DE")>=0) {
		router.delete(basePath+'delete/:id', function(req, res) { cu.delete( model, req, res ); });	
	}
}

router.get('/', house_controller.house_index);

addRouterFor('HouseBSData', House,'IA,CR,CK,GK,GM,GD,SM');
addRouterFor('OsmArea', OsmArea,'IA,CR,CK,GK,GM,GD');
addRouterFor('StyleSheet', StyleSheet,'IA,CR,CK,GK,GM,GD');
addRouterFor('Layout', Layout,'IA,CR,CK,GK,GM,GD');
addRouterFor('PropertyListing', PropertyListing,'IA,CR,CK,GK,GM,GD,FC,DE');
addRouterFor('UIView', UIView,'IA,CR,CK,GK,GM,GD');
addRouterFor('HuMomentsBSData', MLMoments,'IA,CR,CK,GK,GM,GD');
addRouterFor('AppOptions', AppOptions,'IA,CR,CK,GK,GM,GD');
addRouterFor('Calculus', Calculus,'CR,IA,UK,GM');
addRouterFor('CalculusTask', CalculusTask,'CR,IA,UK,GM');
addRouterFor('Material', Material,'CU,UK,NE,GK');
addRouterFor('MaterialColor', MaterialColor,'CR,CK,IA,UK,NE,GK,GM');

// Composites
router.delete('/delete_house/:id', (req, res) => { deleteHouse( req, res ); });	
router.delete('/delete_houses/', (req, res) => { deleteHouses( req, res ); });	

module.exports = router;