var hash = require('object-hash');
var server = require('../bin/www');

var options = { upsert: true, new: false, setDefaultsOnInsert: true };

exports.findOne = function( data, req, res ) {
    const js = JSON.parse( '{ "' + req.params.key + '" : "' + req.params.id + '" }');
    data.findOne( js, function( err, doc) {
        if ( err ) {
            console.log(err);
        } else {
            if ( doc ) {
                res.json( doc );
            } else {
                res.send( '[]' );                
            }
        }
    }).collation({locale: "en", strength: 2});    
}

exports.findMany = function( data, req, res ) {
    var js = '{ "' + req.params.key + '" : "' + req.params.id + '" }';
    data.find( JSON.parse(js), function( err, doc) {
        if ( err ) console.log(err);
        res.json( doc );
    }).collation({locale: "en", strength: 2});    
}

exports.findComplex = function(data, req, res) {
    var complexQuery = req.body["query"];
    data.find( complexQuery, function (err, doc) {
        if (err) console.log(err);
        res.json( doc);
    }).collation({locale: "en", strength: 2});
}

exports.findOneNotExact = function( data, req, res ) {
    var js = { "tags" : req.params.key };
    data.find( js, ( err, doc) => {
        if ( err ) {
            console.log(err);
        } else {
            console.log(js);
            res.json( doc );
        }
    });    
}

exports.distinct = function( data, req, res ) {
    data.distinct( req.params.id, function( err, doc) {
        if ( err ) {
            console.log(err);
        } else {
            res.json( doc );
        }
    });    
}

exports.create_post = function( data, req, res ) {
    req.body['_hash'] = hash(req.body);
    //console.log( req.body );
    data.findOneAndUpdate({ _hash : req.body['_hash'] }, req.body, options, (error, result) => {
        if (error) { 
            console.log(error);  
            res.status(500).send( "[DB ERROR] Table: " + req.url + " failed to upsert" );
        } else {
            console.log(req.url);  
            server.sio.emit( 'message', JSON.stringify({ msg: req.url, name: req.body['name'] }) );
            res.send(req.body);
        }
    });
}

exports.create_and_upload_post = ( data, req, res ) => {
//    req.body['_hash'] = hash(req.body);
    // const split = req.params[0].split("/");
    console.log( req.params );
    const mat = JSON.stringify(req.body['material']);
    res.send( `Body name : ${mat}` );

    // data.findOneAndUpdate({ _hash : req.body['_hash'] }, req.body, options, (error, result) => {
    //     if (error) { 
    //         console.log(error);  
    //         res.status(500).send( "[DB ERROR] Table: " + req.url + " failed to upsert" );
    //     } else {
    //         console.log(req.url);  
    //         server.sio.emit(req.url, JSON.stringify({ name: req.body['name'] }) );
    //         res.send(req.body);
    //     }
    // });
}

exports.create_post_with_key = function( data, req, res ) {
    req.body['_hash'] = hash(req.body);
    var js = '{ "' + req.params.key + '" : "' + req.body[req.params.key] + '" }';
    console.log( js );
    data.findOneAndUpdate(JSON.parse(js), req.body, options, function(error, result) {
        if (error) { console.log(error); return; }
    });
    res.send(req.body);
}

exports.delete = function( data, req, res ) {
    console.log( req.params.id );
    data.findByIdAndDelete( req.params.id, (error, result) => {
        if (error) { 
			console.log("ERROR");
			console.log(error); 
			res.send({result:null, error: error}); 		
		} else {
			if (result!=null) {
				res.send({ result: req.params.id, error: null});
			} else {
				res.send({ result: null, error: null});
			}
		}
    });
}

exports.update_with_query = function( data, req, res ) {
    // req.body['_hash'] = hash(req.body);
	var currentOptions = {
		upsert: false
	};
    console.log( req.body.query );
    data.findOneAndUpdate(req.body.query, req.body.content, currentOptions, function(error, result) {
        if (error) { 
			console.log("ERROR");
			console.log(error); 
			res.send({result:null, error: error}); 		
		} else {
			console.log("UPDATED");
			if (result!=null) {
				res.send({ result: req.body.content, error: null});
			} else {
				res.send({ result: null, error: null});
			}
			//res.send({ result: result}); 
		}
    });
    //res.send(req.body);
}

exports.insert_array_post = function( data, req, res ) {
    for (var i in req.body ) {
        req.body[i]['_hash'] = hash(req.body[i]);
        data.findOneAndUpdate({ _hash : req.body[i]['_hash'] }, req.body[i], options, function(error, result) {
            if (error) { 
				console.log(error); 
				return; 
			}
        });
    }
    res.send(req.body);
}
