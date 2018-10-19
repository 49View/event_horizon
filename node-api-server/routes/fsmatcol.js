//var db = require('../app');

var express = require('express');
var router = express.Router();
var hash = require('object-hash');

var colorMap = require('../models/material_color');

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max));
}

router.get('/get/material_colors/:key', (req, res, next) => {

    const kac = req.params.key.split(" ");
    const ka = kac.map( value => value.toLowerCase() );

    colorMap.distinct( "brand", (error, brandsdb) => {
        if ( !error ) {
            colorMap.distinct( "category", (error, categoriesdb) => {
                if ( !error ) {
                    const brands = brandsdb.map( value => value.toLowerCase() );
                    const categories = categoriesdb.map( value => value.toLowerCase() );

                    const bf = brands.filter(value => -1 !== ka.indexOf(value));
                    const bc = categories.filter(value => -1 !== ka.indexOf(value));
                    const kaft = ka.filter(value => -1 === bf.indexOf(value));
                    const kaf = kaft.filter(value => -1 === bc.indexOf(value));
                    let nameSearch = ".*";
                    let nameCode = "";
                    if ( kaf.length > 0 ) {
                        nameSearch = kaf.reduce( (acc, value) => acc + " (" + value + ")?");
                        nameCode = kaf.reduce( (acc, value) => acc + value);
                    }
                    //const jq = { brand : { $in: bf.length > 0 ? bf : brandsdb }, category : { $in : bc.length > 0 ? bc : categoriesdb }, name: { $regex: nameSearch, $options: 'i'} };
                    const jq = { brand : { $in: bf.length > 0 ? bf : brandsdb }, category : { $in : bc.length > 0 ? bc : categoriesdb }, $or: [ { name: { $regex: nameSearch, $options: 'i'} }, { code : nameCode } ] };
                    console.log( nameSearch );
                    console.log( kaf );
                    console.log( bf );
                    console.log( bc );

                    colorMap.find( jq, (error, colArray) => {
                        if ( error ) {
                            res.status(501).send(`[DB ERROR] Table: MaterialColor - find query ${req.params.key} failed`);  
                            return;
                        } else {
                            let matKey = "W31A"; // Defaults to white if array is empty
                            if ( colArray.length > 0 ) {
                                const ri = getRandomInt(colArray.length);
                                const col = colArray[ri].toObject();
                                matKey = col._id;
                            } 
                
                            colorMap.findOne( { _id : matKey }, (error, result) => {
                                if ( error ) {
                                    res.status(501).send(`[DB ERROR] Table: MaterialColor - find query ${req.params.key} failed`);            
                                    return;
                                } else {
                                    const mc = result.toObject();
                                    const mcs = JSON.stringify(mc);
                                    res.writeHead(200, {
                                        'Content-Type': "text/plain",
                                        'ETag': mc._id,
                                        'Content-Length': mcs.length,
                                    });
                                    res.end(mcs);
                                }
                            });
                        }
                    }).collation({locale: "en", strength: 2});
                
                } else {
                    res.status(501).send(`[DB ERROR] Table: MaterialColor - find query ${req.params.key} failed`);  
                    return;
                }
            });
        } else {
            res.status(501).send(`[DB ERROR] Table: MaterialColor - find query ${req.params.key} failed`);  
            return;
        }
    });
});


module.exports = router;
