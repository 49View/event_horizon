const mongoose = require('mongoose');
const crypto = require('crypto');
const entityModel = require('../models/entity');
const asyncModelOperations = require('../assistants/asyncModelOperations');
const fsController = require('../controllers/fsController');

exports.getMetadataFromBody = (checkGroup, checkRaw, req) => {
    if (req.body===null || !(req.body instanceof Object)) {
        throw "Invalid metadata for entity";
    }
    const metadata = req.body;
    const metadataMissingMessage = "Required attributes missing in metadata:";
    //Check that body contains special attributes used for define entity
    if (checkGroup && typeof(metadata.group)==="undefined") {
        throw metadataMissingMessage + " 'Group'";
    }
    if (typeof(metadata.hash)==="undefined") {
        throw metadataMissingMessage + " 'Hash'";
    }
    if (checkRaw && typeof(metadata.raw)==="undefined") {
        throw metadataMissingMessage + " 'Raw'";
    }
    if ( typeof(metadata.tags)==="undefined" || !(metadata.tags instanceof Array) || metadata.tags.length===0 ) {
        throw metadataMissingMessage + " 'tags'";
    }
    return metadata;
}

exports.cleanupMetadata = (metadata) => {
    const result = {};

    if (typeof(metadata.raw)!=="undefined") {
        result.content = new Buffer(metadata.raw, "base64");
        // metadata.content_id = crypto.createHash('sha256').update(metadata.raw).digest("hex");
    } else {
        result.content = null;
        delete metadata.content_id;
    }
    result.group = metadata.group;
    result.keys = metadata.tags;
    result.public = metadata.public || false;
    result.restricted = metadata.restricted || false;
    //Remove service attributes
    delete metadata.raw;
    delete metadata.group;
    // delete metadata.tags;
    delete metadata.public;
    delete metadata.restricted;
    //Add hash attribute
    result.cleanMetadata = metadata;

    return result;
}

exports.getFilePath = (project, group, content_id) => {
    return project+"/"+group+"/"+content_id;
}

exports.checkFileExists = async (project, group, content_id) => {

    const query = {$and: [{"metadata.content_id":content_id}, {"group":group}, {"project":project}]};
    const result = await entityModel.findOne(query);

    return result!==null?result.toObject():null;
}

exports.createEntity = async (project, group, public, restricted, metadata) => {
    const newEntityDB = new entityModel({ project: project, group: group, public: public, restricted: restricted, metadata: metadata});
    await newEntityDB.save();
    return newEntityDB.toObject();
}

exports.updateEntity = async (entityId, project, group, public, restricted, metadata) => {
    const query = { _id: mongoose.Types.ObjectId(entityId)};

    await entityModel.updateOne(query, { project: project, group: group, public: public, restricted: restricted, metadata: metadata});
}

exports.deleteEntity = async (entityId) => {
    await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId)});
}

exports.deleteEntityComplete = async (project, entity) => {
    currentEntity = entity;
    console.log("[INFO] deleting entity " + currentEntity.metadata.content_id);
    const group = currentEntity.group;
    //Remove current file from S3
    await fsController.cloudStorageDelete( module.exports.getFilePath(project, group, currentEntity.metadata.content_id), "eventhorizonentities");
    //Delete existing entity
    await module.exports.deleteEntity(currentEntity._id);
}

exports.getEntityByIdProject = async (project, entityId, returnPublic) => {
    let query;
    if (returnPublic) {
        query = {$and: [{_id: mongoose.Types.ObjectId(entityId)}, {"$or": [{"project":project}, {"public": true}]}]};
    } else {
        query = {$and: [{_id: mongoose.Types.ObjectId(entityId)}, {"project":project}]};
    }
    const result = await entityModel.findOne(query);

    return result!==null?result.toObject():null;
}

exports.getEntitiesOfProject = async (project, entityId, returnPublic) => {
    let query;
    if (returnPublic) {
        query = [ {"project":project}, {"public": true} ];
    } else {
        query = {"project":project};
    }
    const result = await entityModel.find(query);

    return result!==null?result:null;
}

exports.getEntitiesByProjectGroupTags = async (project, group, tags, version, fullData, randomElements) => {
    const aggregationQueries = [
        {
            $match: {
                $and: [
                    {"metadata.version": Number(version)},
                    {"restricted": false},
                    {"group": group},
                    {
                        "$or": [
                            {"project":project}, 
                            {"public": true}
                        ]
                    },
                    {
                        "metadata.tags": {
                            "$all": tags
                        }
                    }
                ]
            }
        }
    ];

    if (fullData!==true) {
        aggregationQueries.push(
            {   
                $project: {
                    "project": 0,
                    "public": 0,
                    "restricted": 0,
                    "metadata.content_id": 0
                }
            }
        );
    }

    if (randomElements!==null) {
        aggregationQueries.push(
            {
                $sample: {
                    size: randomElements
                }
            }
        );
    }

    const result = await asyncModelOperations.aggregate(entityModel, aggregationQueries);

    return result;
}
