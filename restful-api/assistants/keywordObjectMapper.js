var fsc = require('../controllers/fsController');

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max));
}

function mapKeywords( matMap, k, mappedObject ) {
    for ( const ek of mappedObject.excludedKeywords ) {
        if ( k.search( ek ) != -1 ) {
            return -1;
        }
    }
    const ka = k.split("_");
    for ( const ksa2 of ka ) {
        ksa = ksa2.replace(mappedObject.ext, "");
        if ( ksa.length > 0 ) {
            if ( !matMap[ksa] ) matMap[ksa] = [];
            matMap[ksa].push(k);
        }
    }
    return 0;
}

const mapSingleKeywordIntoDatabase = async ( mo, m, matMap ) => {
    
    try {
        let response = await mo.collection.findOne( { key : m });
        if (response!==null && response.length>0) {
            let mo = result.toObject();
            const mname = matMap[m][0];
            if ( mo.names.find( (elem ) => { return elem = mname; } ) == undefined ) {
                mo.names.push( mname );
                await mo.collection.update({ _id: result._id },{ $set: { names: mo.names }});    
            }
        } else {
            let nm = new mo.collection({ "key" : m, "names" : matMap[m] });
            await nm.save();
        }
    } catch (err) {
        console.log(err);
    }
}

const mapKeywordsIntoDatabase = async ( matMap, mo ) => {
    for ( const m in matMap ) {
        await mapSingleKeywordIntoDatabase( mo, m, matMap );
    }
}

function mapObjectInternal( k, mo ) {
	return new Promise( (resolve, reject) => {
        const p = fsc.cloudStorageFileList( k, false );
        
        p.then( (data) => {
            let matMap = {};
            for ( const k of data ) {
                mapKeywords( matMap, k, mo );
            }
            
            mapKeywordsIntoDatabase(matMap, mo );
            resolve( matMap );
        }).catch( (err) => {
            console.log(err, err.stack);
            reject( err.stack );
        });    
    });
}

// *************************************************************************************************************************************
// ---- Exports ----
// *************************************************************************************************************************************

module.exports = {

uploadObject: function( res, req, mo ) {
    const p = fsc.cloudStorageFileUpdate(req.body, mo.keyname + "s/" + req.params.key);
    p.then( (data) => {
        let matMap = {};
        const needsMapping = mapKeywords( matMap, req.params.key, mo );
        if ( needsMapping != -1 ) {
            mapKeywordsIntoDatabase(matMap, mo );
        }
        res.writeHead(200, { 'ETag': data.ETag } );
        res.end();		  	    		
	}).catch( (err) => {
  	    console.log(err, err.stack);
   		res.status(500).send(`File ${req.params.key} could not be uploaded`)
	});
},

getObject: function( res, req, mo ) {
    const ka = req.params.key.split(/[\s_]+/);

    mo.collection.find( { key : { $in: ka } }, (error, result) => {
        if ( error ) {
            res.status(501).send(`[DB ERROR] Table: ${mo.keyname} - find query ${req.params.key} failed`);            
            return;
        } else {
            if ( result === undefined || result == null || result.length == 0 ) {
                res.status(500).send(`File ${req.params.key} not found`);
                return;
            }    

            let unm = new Set();
            result.map( ( mat ) => {
                const mo = mat.toObject();
                for ( const mn of mo.names ) {
                    let bFound = true;
                    for ( const k of ka ) {
                        if ( mn.search( k ) == -1 ) {
                            bFound = false;
                            break;
                        }
                    }
                    if ( bFound ) {
                        unm.add( mn );
                    }
                }
            });
            
            let ma = Array.from(unm);

            let matKey = mo.defaultName; // Defaults to default if array is empty

            if ( ma.length > 0 ) {
                const ri = getRandomInt(ma.length);
                console.log( ri );
                matKey = ma[ri];

                const p = fsc.cloudStorageFileGet(  mo.keyname + "s/" + matKey );
            
                p.then( (data) => {
                    res.writeHead(200, {
                        'Content-Type': data.ContentType,
                        'Content-Last-Modified': data.LastModified,
                        'ETag': data.ETag,
                        'Content-Length': data.ContentLength,
                        'SourceMap': matKey
                    });
                    res.end(data["Body"]);
                }).catch( (err) => {
                    console.log(err, err.stack);
                    res.status(500).send(`File ${req.params.key} not found`);
                });    
            } else {
                // console.log(err, err.stack);
                res.status(500).send(`File ${req.params.key} not found`);
            }

        }
    } );
},

mapObject: function( k, res, mo ) {
    const p = mapObjectInternal( k, mo );
    p.then( (data) => {
        res.json(data);
    }).catch( (err) => {
        res.status(500).send(`${k} cannot be mapped`);
    });
},

mapObjects: function( res, mo ) {
    const pkeys = fsc.cloudStorageFileList( mo.keyname + "s", false );
    pkeys.then( (data) => {
        mo.collection.remove( {}, (err, result) => {
            if (err) {
                writeError( res, 503, err, `Table ${mo.keyname} failed to remove`);
            }
        } );
        if ( Array.isArray(data) && data.length > 0 ) {
            let matMap = {};
            for ( const k of data ) {
                mapKeywords( matMap, k, mo );
            }
            mapKeywordsIntoDatabase(matMap, mo );
        }
        res.send(data);
    }, (err) => {
        writeError( res, 404, err, `Mapping ${mo.keyname} not found`);
    });
},

MapObject: class {
    constructor( coll, kn, ext, ec, hasDefault, defaultName ) {
        this.collection = coll;
        this.keyname = kn;
        this.excludedKeywords = ec;
        this.ext = ext;
        this.hasDefault = hasDefault;
        this.defaultName = defaultName;
    }
}

}