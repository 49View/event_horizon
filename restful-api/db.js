const mongoose = require("mongoose");
const mongodb = require("mongodb");
const delay = require('delay');
const md5 = require("md5");
const streamifier = require('streamifier');
const stream = require('stream');
const util = require('util');

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
            exports.bucketSourceAssets = new mongodb.GridFSBucket(mongoose.connection.client.db('event_horizon'), { bucketName: "fs_source_assets"});
            exports.bucketEntities = new mongodb.GridFSBucket(mongoose.connection.client.db('event_horizon'), { bucketName: "fs_entities"});
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
    memStore[key] = new Buffer(''); // empty
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

exports.fsExists = ( bucketFSModel, filename, metadata ) => {
    return bucketFSModel.find( { filename: filename, ...metadata } );
}

exports.fsEqual = async ( bucketFSModel, filename, data, metadata ) => {
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
    return ret;
}

exports.fsDelete = async ( bucketFSModel, id ) => {
    await bucketFSModel.delete( id );
}

exports.fsInsert = async( bucketFSModel, filename, data, metadata ) => {
    return new Promise( (resolve, reject) => {
        streamifier.createReadStream(data).pipe(bucketFSModel.openUploadStream(filename, {
            metadata: metadata,
            disableMD5: true
        })).on('error', reject ).on('finish', resolve );
    });
}

exports.fsUpsert = async ( bucketFSModel, filename, data, metadata, metadataComp ) => {

    let ret = 200;
    const existAsset = module.exports.fsExists( bucketFSModel, filename, metadataComp );
    if ( existAsset ) {
        const res = await existAsset.toArray();
        for ( const elem of res ) {
            if ( ! await module.exports.fsEqual( bucketFSModel, filename, data, metadataComp ) ) {
                await module.exports.fsDelete( bucketFSModel, elem._id );
            } else {
                ret = 204;
            }
        }
    }
    if ( ret === 200 ) {
        await module.exports.fsInsert( bucketFSModel, filename, data, metadata );
    }
    return ret;
}
