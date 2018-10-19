const url = require('url');
const path = require('path');
const cheerio = require('cheerio');
//var http = require('http');
//const https = require('https');
const axios = require('axios');
var server = require('../bin/www');
var cloudApi = require('../third_party_api/s3');
var propertyListingModel = require('../models/property_listing');
const async = require('async');
//const parallel = require('async-await-parallel');

const regexMatch = (regex, text, requiredMatches, matchIndex) => {
    
    //console.log("Search for "+matchIndex+" required "+requiredMatches, regex);
    let result = null;
    const matches = regex.exec(text);
    //if (matches!==null) console.log("Matches: ", matches.length);
    //console.log(matches);
    // let m;

    // while ((m = regex.exec(text)) !== null) {
    //     // This is necessary to avoid infinite loops with zero-width matches
    //     if (m.index === regex.lastIndex) {
    //         regex.lastIndex++;
    //     }
        
    //     // The result can be accessed through the `m`-variable.
    //     m.forEach((match, groupIndex) => {
    //         console.log(`Found match, group ${groupIndex}: ${match}`);
    //     });
    // }

    if (matches!==null && matches.length===requiredMatches) {
        console.log("FOUND");
        result = matches[matchIndex];
    } else {
        console.log("NOT FOUND");
    }
    return result;
}

const cleanString = (source) => {
    if (source!==null) {
        return source.replace(/\x0d/g,'').replace(/\x0a/g,'').replace(/\x09/g,'').replace(/[^\x00-\x7f]/g,"").trim();
    } else {
        return null;
    }
}

const scrapRightMoveFloorplan = (htmlUrl, htmlSource) => {

    let floorplansString;
    let floorplansArray;
    let imagesString;
    let imagesArray;
    let result;
    let regex;
    let latitude, longitude;

    result={
        name: null,
        addressLine1: null,
        description: null,
        price: null,
        priceReadable: null,
        priceUnity: null,
        floorplanUrl: null,
        images: null,
        location: null,
        keyFeatures: [],
        origin: htmlUrl
    };

    $ = cheerio.load(htmlSource);

    
    //
    // TITLE
    //
    console.log("TITLE SEARCH");
    result.name=cleanString($('h1[class=fs-22]').text());

    //
    // ADDRESS
    //
    console.log("ADDRESS SEARCH");
    result.addressLine1=cleanString($('h1[class=fs-22]').parent().find('address[itemprop=address]').text());

    //
    // DESCRIPTION
    //
    console.log("DESCRIPTION SEARCH");
    result.description=cleanString($('p[itemprop=description]').text());

    //
    // PRICE
    //
    console.log("PRICE SEARCH");
    let price=cleanString($('p[id=propertyHeaderPrice]').find('strong').text());
    result.priceReadable=price;
    result.priceUnity="pound";
    price=price.replace(/,/g,"").replace(/£/g,"").trim();
    if (isNaN(price)) {
        result.price=-1;
    } else {
        result.price=Number(price);
    }

    //
    //KEY FEATURES
    //
    console.log("KEY FEATURES");
    const kfElements = $("div[class='sect key-features'] > ul li");
    for (let i=0;i<kfElements.length;i++)
    {
        result.keyFeatures.push(cleanString( $(kfElements[i]).text() ) );
    }

    //
    // LATITUDE
    //
    //Extract string array from images:
    console.log("LATITUDE SEARCH");
    regex = /"latitude":(\-?\d*\.?\d*)/mg
    latitude=regexMatch(regex, htmlSource, 2, 1);

    //
    // LONGITUDE
    //
    //Extract string array from images:
    console.log("LONGITUDE SEARCH");
    regex = /"longitude":(\-?\d*\.?\d*)/mg
    longitude=regexMatch(regex, htmlSource, 2, 1);

    if (latitude!==null && longitude!==null) {
        result.location = {
            type: "Point",
            coordinates: [+longitude,+latitude]
        };
    }

    //
    // IMAGES
    //
    //Extract string array from images:
    console.log("IMAGES SEARCH");
    regex = /images[\s]*\:[\s]*(\[[^\]]+\])/mg
    imagesString = regexMatch(regex, htmlSource, 2, 1);
    if (imagesString!==null) {
        imagesArray = JSON.parse(imagesString);
        if (imagesArray!=null && imagesArray.constructor === Array) {
            result.images = imagesArray;
        }
    }

    //
    // FLOORPLAN
    //
    //Extract string array from zoomUrls
    console.log("FLOORPLAN TYPE 1 SEARCH");
    regex = /zoomUrls\:\s*(\[[^\]]+\])/mg
    floorplansString = regexMatch(regex, htmlSource, 2, 1);
    if (floorplansString!==null) {
        floorplansArray = JSON.parse(floorplansString);
        if (floorplansArray!=null && floorplansArray.constructor === Array) {
            result.floorplanUrl = floorplansArray[floorplansArray.length-1];
        }
    }
    if (result.floorplanUrl===null) {
        console.log("FLOORPLAN TYPE 2 SEARCH");
        regex = /<img[^>]+src="([^">]+)"[^>]*class="site-plan"/mg
        result.floorplanUrl = regexMatch(regex, htmlSource, 2, 1);
    }

    //console.log("Scraping result");
    //console.log(result);

    return result;
}

const prepareDownloadOptions = (url, method, responseType) => {
    return {
        url: url,
        method: method,
        responseType: responseType,
        headers: {
            "User-Agent": 'Chrome/60.0.3112.113',
            "Content-Type": "text/html"
        }
    }
}

const downloadFileAsync = async (url, method, responseType, isBinary) => {
    
    const downloadOptions = prepareDownloadOptions(url, method, responseType);

    let response=null;
    let content = null;

    try {
        response = await axios(downloadOptions);
    }
    catch (err) {
        console.log("Error "+ err.response.statusText +" downloading file "+url);
    }

    if (response!==null) {

        if (!isBinary) {
            content = response.data;                
        } else {
            content = new Buffer(response.data, 'binary');
        }
    } else {
        console.log("File "+url+" download ERROR");
    }

    return content;
}

const getPropertyByOrigin = async ( originUrl ) => {
    
    let property = null;
    try {
        let response = await propertyListingModel.find( {origin : originUrl }).limit(1);
        if (response!==null && response.length>0) {
            property = response[0];
        }
    } catch (err) {
        console.log("Error finding property "+originUrl);
        console.log(err);
    }

    return property;
}

const saveProperty = async (propertySource) => {

    let savedProperty = null;
    let propertyModel = new propertyListingModel(propertySource);
    try {
        savedProperty = await propertyModel.save();
    } catch (err) {
        console.log("Error saving property "+propertySource);
        console.log(err);
    }

    //console.log(savedProperty);

    return savedProperty;
    //return (savedProperty!==null)?saveProperty.toObject():null;
}

const updateProperty = async (originUrl, property) => {

    //console.log(property);
    try {
        let query = { origin : originUrl };
        console.log(query);
        await propertyListingModel.findOneAndUpdate( query, property, { upsert: false});
        //console.log(result);
        console.log("Property updated");
    } catch (err) {
        console.log("Error updating property "+property);
        console.log(err);
    }
}

const uploadFileToS3 = async (path, content) => {

    let result = false;
    try {
        cloudApi.upload(content, path);
        result = true;
    } catch (err) {
        console.log("Error uploading file "+path);
        console.log(err);
    }

    return result;
}

const copyFile = (url, method, responseType, isBinary, targetPath, type, id, callback) => {
    
    const downloadOptions = prepareDownloadOptions(url, method, responseType);

    let content = null;

    axios(downloadOptions).then( response => {
        //console.log("Downloaded file "+url);
        if (response!==null) {
            if (!isBinary) {
                content = response.data;                
            } else {
                content = new Buffer(response.data, 'binary');
            }
        }

        cloudApi.upload(content, targetPath).then( () => {
            //console.log("Uploaded file "+targetPath);
            callback(null, { id: id, type: type, url: targetPath, status: 'OK'});
        }).catch( (err) => {
            console.log("Error uploading file "+targetPath);
            callback(null, { id: id, status: 'KO', error: "Upload error"});
        })
    }).catch( err => {
        console.log("Error downloading file "+url);
        callback(null, { id: id, status: 'KO', error: "Download error"});
    });
}

const copyMultifiles = async (copyInfo) => {    

    return new Promise( (resolve, reject) => {
        async.map(copyInfo,
            (options, doneCallback) => {
                copyFile(options.url, options.method, options.responseType, true, options.targetPath, options.type, options.id, doneCallback);
            },
            (err, results) => {
                //console.log("Finished");
                resolve(results);
            }
        );
    });

}

const getPropertyImages = async (propertyId, imagesInfo) => {

    let targetPath;
    //await getPropertyImage(0, imagesInfo[0]);
    const imageCopyOptions = [];
    for (let i=0;i<imagesInfo.length;i++) {
        // targetPath="images/properties/"+propertyId+"/images/thumbnail-"+ i + path.extname(imagesInfo[i].thumbnailUrl);
        // imageCopyOptions.push({
        //     url: imagesInfo[i].thumbnailUrl,
        //     method: "GET",
        //     responseType: "arraybuffer",
        //     targetPath: targetPath,
        //     type: "thumbnail",
        //     id: i
        // });
        targetPath="images/properties/"+propertyId+"/images/master-"+ i + path.extname(imagesInfo[i].masterUrl);
        imageCopyOptions.push({
            url: imagesInfo[i].masterUrl,
            method: "GET",
            responseType: "arraybuffer",
            targetPath: targetPath,
            type: "master",
            id: i
        });
    }

    let copyResults =null;
    copyResults = await copyMultifiles(imageCopyOptions);

    results=[];

    for (let i=0;i<copyResults.length;i++) {
        let currValue=copyResults[i];
        if (currValue.status==="OK") {
            let element = results.find( v => v.id===currValue.id);
            if (element===undefined) {
                element = {};
                element.id=currValue.id;
                element.caption=imagesInfo[currValue.id].caption;
                element[currValue.type]=currValue.url;
                results.push(element);
            } else {
                element[currValue.type]=currValue.url;
            }
        }
    }
    //Elaborate results
    //console.log(results.filter(v => v.thumbnail!==undefined && v.master!==undefined));

    return results.filter(v => v.master!==undefined);
}

function emitFloorplan2d3d( res, doc ) {
    const json = { msg : 'floorplan2dto3d', property_listing : doc };
    server.sio.emit('message', JSON.stringify(json) );
    res.contentType('application/json');
    json.error=false;
    res.json(json);
}

exports.create_post = async ( req, res ) => {

    let errorMessage = null;
    const sourceUrl = url.parse(req.body.url).href;
    //console.log(parsedUrl);
    //CHECK IF PROPERTY EXISTS ON DB
    let property = await getPropertyByOrigin(sourceUrl);
    //console.log("PROPERTY "+(property==null?'NOT FOUND':'FOUND'));
    if (property===null) {
        //DOWNLOAD HTML SOURCE PROPERTY
        let sourceContent = await downloadFileAsync(sourceUrl, "GET", "text", false);
        //console.log("DOWNLOADED FILE", sourceContent)
        if (sourceContent!==null) {
            //SCRAPING SOURCE PROPERTY HTML
            let propertySource = scrapRightMoveFloorplan(sourceUrl, sourceContent);
            let floorplanUrl = propertySource.floorplanUrl;
            let propertyImagesInfo = propertySource.images.slice(0);
            //propertySource.images=propertyImagesInfo.map((e,i) => { return { id: i, caption: e.caption} });
            // console.log(propertySource);
            // console.log(propertyImagesInfo);
            //let result = await getPropertyImages("TEST",propertyImagesInfo);
            delete propertySource.floorplanUrl;
            delete propertySource.images;

            // res.contentType('application/json');
            // res.send(propertySource);
            // return;

            if (floorplanUrl!==null) {
                //SAVE PROPERTY ON DB
                let savedProperty = await saveProperty(propertySource);
                if (savedProperty!==null) {
                    //DOWNLOAD FLOORPLAN CONTENT
                    let floorplanContent = await downloadFileAsync(floorplanUrl, "GET", "arraybuffer", true);
                    if (floorplanContent!==null) {
                        //UPLOAD FLOORPLAN TO S3
                        const floorplanS3Path = "images/properties/"+savedProperty._id.toString()+"/floorplanImage" + path.extname(floorplanUrl);
                        //console.log("Upload floorplan to "+floorplanS3Path);
                        if (await uploadFileToS3(floorplanS3Path, floorplanContent)) {
                            //PROPERTY READY
                            property = savedProperty.toObject();
                            //COPY PROPERTY IMAGES
                            //console.log(sp);
                            property.images = await getPropertyImages(property._id.toString(), propertyImagesInfo);
                            await updateProperty(property.origin, property);
                        } else {
                            errorMessage="Error uploading floorplan image";
                        }
                    } else {
                        errorMessage="Error downloading floorplan image";
                    }
                } else {
                    errorMessage="Error saving property";
                }
            } else {
                errorMessage="Floorplan not found";
            }
        } else {
            errorMessage="Error downloading source";
        }
    }
    if (property!==null) {
        emitFloorplan2d3d( res, property );
    } else {
        const result = {
            error: true,
            message: errorMessage
        };
        emitFloorplan2d3d( res, result );
        res.contentType('application/json');
        res.send(result);
    }
}
