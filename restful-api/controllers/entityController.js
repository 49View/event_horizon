const fsc = require('../controllers/fsController');
const path = require('path');
const express = require('express');
const mongoose = require('mongoose');
const crypto = require('crypto');
const router = express.Router();
const entityModel = require('../models/entity');
const entityKeyModel = require('../models/entity_key');
const asyncModelOperations = require('../assistants/asyncModelOperations');


exports.getMetadataFromBody = (checkGroup, checkRaw, req) => {
    if (req.body===null || !(req.body instanceof Object)) {
        throw "Invalid metadata for entity";
    }
    const metadata = req.body;
    //Check that body contains special attributes used for define entity
    if ((checkGroup && typeof(metadata.group)==="undefined")
       || (checkRaw && typeof(metadata.raw)==="undefined")
       || typeof(metadata.tags)==="undefined"
       || !(metadata.tags instanceof Array)
       || metadata.tags.length===0) {
        throw "Required attributes missing in metadata";
    }
    return metadata;
}

exports.cleanupMetadata = (contentHash, metadata) => {
    const result = {};

    if (contentHash===null) {
        result.content = new Buffer(metadata.raw, "base64");
        metadata.contentHash = crypto.createHash('sha256').update(metadata.raw).digest("hex");
    } else {
        metadata.contentHash = contentHash;
    }
    result.group = metadata.group;
    result.keys = metadata.tags;
    result.public = metadata.public || false;
    //Remove service attributes
    delete metadata.raw;
    delete metadata.group;
    delete metadata.tags;
    delete metadata.public;
    //Add hash attribute
    result.cleanMetadata = metadata;

    console.log(result);
    return result;
}

// const sendFile = ( res, data ) => {
//     res.writeHead(200, {
//         'Content-Type': data.ContentType,
//         'Content-Last-Modified': data.LastModified,
//         'ETag': data.ETag,
//         'Content-Length': data.ContentLength
//     });
//     res.end(data["Body"]);
// }

// const getFilePath = (group, filename) => {
//     return "entities/"+group+"/"+filename;
// }

// // const checkFileExists = async (group, filename) => {

// //     let result = null;
// //     try {
// //         const query = {$and: [{"metadata.file":filename}, {"group":group}]};
// //         //const query = { "metadata.file": key};
// //         console.log(query);
// //         result=await entityModel.findOne(query);
// //         console.log(result);
// //     } catch (ex) {
// //         console.log("checkFileExists", ex);
// //         result=null;
// //     }
// //     return result;
// // }

// const checkNameExists = async (group, name) => {

//     let result = null;
//     try {
//         const query = {$and: [{"metadata.keyname":name}, {"group":group}]};
//         result=await entityModel.findOne(query);
//     } catch (ex) {
//         result=null;
//     }
//     return result;
// }

// const getKeysFromName = (group, name) => {

//     const keys = name.split(/[\s_\.]+/).map(k => k.toLowerCase());
//     if (group!==null) {
//         keys.push("group_"+group);
//     }

//     console.log("KEYS: ",keys);
//     return keys;
// }

// // const getKeysFromFilename = (group, filename) => {

// //     const keys = path.parse(filename).name.split(/[\s_\.]+/).map(k => k.toLowerCase());
// //     keys.push("group_"+group);

// //     console.log("KEYS: ",keys);
// //     return keys;
// // }

// // const createEntityForFile = async (group, filename) => {
// //     const newEntityDB = new entityModel({ group: group, metadata: {file: filename, name: path.parse(filename).name}});
// //     await newEntityDB.save();
// //     return newEntityDB.toObject();
// // }


// // const createEntityForNoFile = async (group, name, metadata) => {
// //     const entityMetaData = { ...metadata
// //         , name: name
// //     };
// //     const newEntityDB = new entityModel({ group: group, metadata: entityMetaData});
// //     await newEntityDB.save();
// //     return newEntityDB.toObject();
// // }

// const createEntityKeysRelations = async(entityId,keys) => {

//     const upsertOps = [];
//     keys.forEach(key => {
//         upsertOps.push(
//             {
//                 updateOne: {
//                     filter: { key: key},
//                     update: {
//                         "$addToSet": {"entities": mongoose.Types.ObjectId(entityId)},
//                         "$setOnInsert": { "key": key}
//                     },
//                     upsert: true
//                 }
//             }
//         );
//     });
//     console.log("UPSERT OPS:", JSON.stringify(upsertOps));
//     let result=null;
//     try {
//         result=await entityKeyModel.bulkWrite(upsertOps, {ordered: false});
//     } catch (ex) {
//         console.log("ERROR:",ex);
//         //console.log(ex.writeErrors[0].err);
//         result=null;
//     }
//     return result;
// }

// const deleteEntity = async (entityId) => {
//     let result=null;
//     try {
//         result=await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId)});
//         console.log("RESULT: ", result.data);
//     } catch (ex) {
//         console.log("ERROR:",ex);
//         //console.log(ex.writeErrors[0].err);
//         throw ex;
//     }
//     return result;
// }

// const deleteEntityKeys = async (entityId) => {
//     const updateOps = [
//         {
//             updateMany: {
//                 filter: { entities: { $elemMatch: { $eq: mongoose.Types.ObjectId(entityId) }}},
//                 update: {
//                     $pull: { entities: mongoose.Types.ObjectId(entityId)}
//                 }
//             }
//         },
//         {   
//             deleteMany: {
//                 filter: { entities: { $size: 0 }}
//             }
//         }
//     ];
//     let result=null;
//     try {
//         result=await entityKeyModel.bulkWrite(updateOps, {ordered: true});
//         console.log("RESULT: ", result.data);
//     } catch (ex) {
//         console.log("ERROR:",ex);
//         //console.log(ex.writeErrors[0].err);
//         throw ex;
//     }
//     return result;
// }

// const updateEntity = async (filterQuery, updateQuery) => {

//     let result=null;
//     try{
//         result=await entityModel.updateOne(filterQuery, updateQuery);
//         console.log(result);        
//     } catch (ex) {
//         console.log("ERROR:",ex);
//         throw ex;
//     }

//     return result;
// }

// const addMetadataToEntityById = async (entityId, metadata) => {

//     const newMetadata = {};

//     for (let m in metadata) {
//         newMetadata["metadata."+m]=metadata[m];
//     }
//     const filterQuery = { _id: mongoose.Types.ObjectId(entityId) };
//     const updateQuery = { $set: newMetadata };

//     return await updateEntity(filterQuery, updateQuery);
// }

// const addMetadataToEntityByGroupAndName = async (group, name, metadata) => {

//     const newMetadata = {};

//     for (let m in metadata) {
//         newMetadata["metadata."+m]=metadata[m];
//     }
//     const filterQuery = { group: group, "metadata.keyname": name };
//     const updateQuery = { $set: newMetadata };

//     return await updateEntity(filterQuery, updateQuery);
// }

// const addMetadataToEntityByGroupAndFilename = async (group, filename, metadata) => {

//     const newMetadata = {};

//     for (let m in metadata) {
//         newMetadata["metadata."+m]=metadata[m];
//     }
//     const filterQuery = { group: group, "metadata.file": filename };
//     const updateQuery = { $set: newMetadata };

//     return await updateEntity(filterQuery, updateQuery);
// }

// const prepareAggregationQuery = (keys) => {

//     return [
//         { $match: { 
//             key: {$in: keys } 
//         }},
//         { $group: {
//             _id: 0,
//             "sets": {
//                 $push: "$entities"
//             },
//             "initialSet": {
//                 $first: "$entities"
//             }
//         }},
//         { $project: {
//             "result": {
//                 $reduce: {
//                     input: "$sets",
//                     initialValue: { $cond: { if: { $eq: [{$size:"$sets"}, keys.length]}, then: "$initialSet", else:[] }},
//                     in: { "$setIntersection": [ "$$value", "$$this"] }
//                 }
//             }            
//         }},
//         { $unwind: {
//                 path: "$result"
//         }}
//     ]

// }

// const getEntities = async (req, res, randomElementNumber) => {
//     //Prepare keys
//     const keys = req.params.keys.split(",");
//     keys.push("group_"+req.params.group);

//     const aggregationQuery = prepareAggregationQuery(keys);
//     if (randomElementNumber!==null) {
//         aggregationQuery.push(
//             { $sample: {
//                 size: randomElementNumber
//             }}
//         );

//     }
//     aggregationQuery.push(
//         { $lookup: {
//             from: "entities",
//             localField: "result",
//             foreignField: "_id",
//             as: "entities"
//         }}
//     );
//     aggregationQuery.push(
//         { $project: {
//             _id: 0,
//             entity: { $arrayElemAt: ["$entities",0] }
//         }}
//     );
//     // aggregationQuery.push(
//     //     { $unwind: {
//     //         path: "$entity"
//     //     }}
//     // );
//     aggregationQuery.push(
//         { $replaceRoot: { 
//             newRoot: "$entity.metadata" 
//         }}
//     );

//     return await asyncModelOperations.aggregate(entityKeyModel, aggregationQuery);
    
// }

// const deleteId = async (eId) => {

//     let result = { entity: null, entityKeys: null, error : null};
//     let entity = {};

//     entity = await entityModel.findOne({ _id: mongoose.Types.ObjectId(eId)});

//     if (entity!==null) {
//         entity=entity.toObject();
//     }

//     try {
//         if (entity===null) {
//             throw "Entity doesn't exist";
//         }
//         if (entity.metadata.file!==null) {
//             const path = getFilePath(entity.group, entity.metadata.file);
//             await fsc.cloudStorageDelete(path);
//         }
//         result.entity=await deleteEntity(eId);
//         result.entityKeys = await deleteEntityKeys(eId);
//     }
//     catch (ex) {
//         result.error=ex;
//     }

//     return result;
// }
