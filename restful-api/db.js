const mongoose = require("mongoose");
const mongodb = require("mongodb");
const delay = require('delay');
const md5 = require("md5");
const streamifier = require('streamifier');
const stream = require('stream');
const util = require('util');
const logger = require('./logger');

//Set up default mongoose connection

exports.initDB = async () => {
    const dbMaxConnectionTimeSeconds = 300;
    const dbConnectionRetryInterval = 55;
    let dbConnectionTimeElaped = 0;
    // const mongoDBUrl = `mongodb://mongo1:27017,mongo2:27017,mongo3:27017/event_horizon?replicaSet=rs0`;
    const mongoDBUrl = `mongodb://localhost:27017/event_horizon?replicaSet=rs0`;

    while (dbConnectionTimeElaped < dbMaxConnectionTimeSeconds) {
        try {
            await mongoose.connect(mongoDBUrl, {
                useNewUrlParser: true,
                useUnifiedTopology: true
            });
            exports.bucketSourceAssets = new mongodb.GridFSBucket(mongoose.connection.client.db('event_horizon'), { bucketName: "fs_assets_to_elaborate"});
            exports.bucketEntities = new mongodb.GridFSBucket(mongoose.connection.client.db('event_horizon'), { bucketName: "fs_entity_assets"});
            console.log("MongoDB connected with GridFS");

            return mongoose.connection;
        } catch (err) {
            console.log(err);
            await delay(dbConnectionRetryInterval*1000);
            dbConnectionTimeElaped+=dbConnectionRetryInterval;
        }
    }
    return null;
}

class FindResults {
    constructor() {
        this.bFileExist = false;
        this.bFileIsSame = false;
    }
}

let Writable = stream.Writable;
let memStore = { };

/* Writable memory stream */
function WMStrm(key, options) {
    // allow use without new operator
    if (!(this instanceof WMStrm)) {
        return new WMStrm(key, options);
    }
    Writable.call(this, options); // init super
    this.key = key; // save key
    memStore[key] = new Buffer.from(''); // empty
}
util.inherits(WMStrm, Writable);

WMStrm.prototype._write = function (chunk, enc, cb) {
    // our memory store stores things in buffers
    let buffer = (Buffer.isBuffer(chunk)) ?
        chunk :  // already is Buffer use it
        new Buffer.from(chunk, enc);  // string, convert

    // concat to the buffer already there
    memStore[this.key] = Buffer.concat([memStore[this.key], buffer]);
    cb();
};

const memoryPromise = async ( bucketFSModel, filename, callback ) => {
    return new Promise(resolve => {
        let wstream = new WMStrm('file');
        wstream.on('finish', () => {
            callback();
            resolve();
        } );
        bucketFSModel.openDownloadStreamByName(filename).pipe(wstream);
    });
}

exports.fsExists = async ( bucketFSModel, filename, metadata ) => {
    try {
        const ret = bucketFSModel.find( { filename: filename, ...metadata } );
        const entries = await ret.toArray();
        logger.info( "Filename " + filename + (entries.length > 0 ? " already exists" : " not found") );
        return entries;
    } catch (e) {
        logger.error("fsExists of: " + filename + " failed because " + e );
        return null;
    }
}

exports.fsEqual = async ( bucketFSModel, filename, data, metadata ) => {
    try {
        const queryLength = { filename: filename, ...metadata, length: data.length };
        const existSameLengthAsset = bucketFSModel.find( queryLength );
        let ret = false;
        if ( existSameLengthAsset ) {
            await memoryPromise(bucketFSModel, filename, () => {
                if ( md5(memStore.file) === md5(data) ) {
                    ret = true;
                }
            });
        }
        logger.info("Exact copy of " + filename + ( ret ? "" : " NOT" ) + " found");
        return ret;
    } catch (e) {
        logger.error("fsEqual of: " + filename + " failed because " + e );
        return null;
    }
}

exports.fsDelete = async ( bucketFSModel, id ) => {
    try {
        logger.info("Deleting asset: " + id.toString() );
        await bucketFSModel.delete( id );
    } catch (e) {
        logger.error("fsDelete of: " + id + " failed because " + e );
    }
}

exports.fsInsert = async( bucketFSModel, filename, data, metadata ) => {
    try {
        logger.info("Inserting new asset: " + filename );
        return new Promise( (resolve, reject) => {
            streamifier.createReadStream(data).pipe(bucketFSModel.openUploadStream(filename, {
                metadata: metadata,
                disableMD5: true
            })).on('error', reject ).on('finish', resolve );
        });
    } catch (e) {
        logger.error("fsInsert of: " + filename + " failed because " + e );
    }
}

exports.fsUpsert = async ( bucketFSModel, filename, data, metadata, metadataComp ) => {

    try {
        logger.info("Performing upsert...");
        let bPerformInsert = true;
        const entries = await module.exports.fsExists( bucketFSModel, filename, metadataComp );
        for ( const elem of entries ) {
            const bIsExactCopy = await module.exports.fsEqual( bucketFSModel, filename, data, metadataComp );
            if ( !bIsExactCopy ) {
                await module.exports.fsDelete( bucketFSModel, elem._id );
            }
            bPerformInsert &= !bIsExactCopy;
        }
        if ( bPerformInsert ) {
            await module.exports.fsInsert( bucketFSModel, filename, data, metadata );
        } else {
            logger.info("No upsert operation performed of " + filename + " because there's already an exact binary copy present");
        }
        return bPerformInsert;
    } catch (e) {
        logger.error("fsUpsert of: " + filename + " failed because " + e);
        return false;
    }
}

