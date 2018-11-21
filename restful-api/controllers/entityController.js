const mongoose = require('mongoose');
const crypto = require('crypto');
const entityModel = require('../models/entity');
const asyncModelOperations = require('../assistants/asyncModelOperations');


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
        metadata.contentHash = crypto.createHash('sha256').update(metadata.raw).digest("hex");
    } else {
        result.content = null;
        delete metadata.contentHash;
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

exports.getFilePath = (project, group, contentHash) => {
    return project+"/"+group+"/"+contentHash;
}

exports.checkFileExists = async (project, group, contentHash) => {

    const query = {$and: [{"metadata.contentHash":contentHash}, {"group":group}, {"project":project}]};
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

exports.getEntitiesByProjectGroupTags = async (project, group, tags, fullData, randomElements) => {
    const aggregationQueries = [
        {
            $match: {
                $and: [
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
                    "group": 0,
                    "public": 0,
                    "restricted": 0,
                    "metadata.contentHash": 0
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
