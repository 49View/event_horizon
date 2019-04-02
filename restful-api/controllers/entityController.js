const mongoose = require('mongoose');
const zlib = require('zlib');
const entityModel = require('../models/entity');
const asyncModelOperations = require('../assistants/asyncModelOperations');
const fsController = require('../controllers/fsController');
const tar = require('tar-stream');
const streams = require('memory-streams');

const getMetadataFromBody = (checkGroup, checkRaw, req) => {
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

const createEntityFromMetadata = async (project, metadata) => {
    const { content, group, isPublic, isRestricted, cleanMetadata } = cleanupMetadata(metadata);
    let filePath=getFilePath(project, group, cleanMetadata.name);
    //Check content exists in project and group
    const copyEntity = await checkFileExists(project, group, cleanMetadata.hash)
    if (copyEntity===null) {
        //Upload file to S3
        let savedFilename = {"changed":false, "name": filePath};
        await fsController.cloudStorageGetFilenameAndDuplicateIfExists( filePath, "eventhorizonentities", savedFilename );
        if ( savedFilename['changed'] == true ) {
            const nn = savedFilename["name"];
            cleanMetadata["name"] = nn.substring( nn.lastIndexOf("/")+1, nn.length);
        }
        filePath = savedFilename["name"];
        await fsController.cloudStorageFileUpload(content, filePath, "eventhorizonentities" );
        //Create entity
        return await createEntity(project, group, isPublic, isRestricted, cleanMetadata);
    }
    //Create entity
    return null;
}

const createEntitiesFromContainer = async (project, containerBody ) => {
    let container = tar.extract();
    const metadatas = [];
    const entities = [];

    container.on('entry', function(header, stream, next) {
        console.log( header.name );
        var writer = new streams.WritableStream();
        stream.pipe(writer);   
        stream.on('end', function() {
            console.log( writer.toString() );
            metadatas.push(writer.toString());
            next() // ready for next entry
        })       
        stream.resume() // just auto drain the stream
    });

    const deflatedBody = zlib.inflateSync(new Buffer.from(containerBody));
  
    var reader = new streams.ReadableStream(deflatedBody);
    await reader.pipe(container);
    console.log( "**************");
    reader.on('finish', () => {
        console.log( "############### ");
        // all entries read
        // res.status(200).send(null);
      });

    metadatas.forEach(element => {
        console.log( "############### ");
      // const newEntity = await createEntityFromMetadata( project, element );
      // if ( newEntity !== null ) entities.push(newEntity);            
    });

    return entities;
    // console.log(deflatedBody);
}

const cleanupMetadata = (metadata) => {
    const result = {};

    if (typeof(metadata.raw)!=="undefined") {
        result.content = zlib.inflateSync(new Buffer.from(metadata.raw, "base64"));
    } else {
        result.content = null;
    }
    result.group = metadata.group;
    result.keys = metadata.tags;
    result.isPublic = metadata.isPublic || false;
    result.isRestricted = metadata.isRestricted || false;
    //Remove service attributes
    delete metadata.raw;
    delete metadata.group;
    delete metadata.isPublic;
    delete metadata.isRestricted;
    //Add hash attribute
    result.cleanMetadata = metadata;

    return result;
}

const getFilePath = (project, group, name) => {
    return project+"/"+group+"/"+name;
}

const checkFileExists = async (project, group, hash) => {

    const query = {$and: [{"metadata.hash":hash}, {"group":group}, {"project":project}]};
    const result = await entityModel.findOne(query);

    return result!==null?result.toObject():null;
}

const createEntity = async (project, group, isPublic, isRestricted, metadata) => {
    const newEntityDB = new entityModel({ project: project, group: group, isPublic: isPublic, isRestricted: isRestricted, metadata: metadata});
    await newEntityDB.save();
    return newEntityDB.toObject();
}

const updateEntity = async (entityId, project, group, isPublic, isRestricted, metadata) => {
    const query = { _id: mongoose.Types.ObjectId(entityId)};

    await entityModel.updateOne(query, { project: project, group: group, isPublic: isPublic, isRestricted: isRestricted, metadata: metadata});
}

const deleteEntity = async (entityId) => {
    await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId)});
}

const deleteEntityComplete = async (project, entity) => {
    currentEntity = entity;
    console.log("[INFO] deleting entity " + currentEntity.metadata.name);
    const group = currentEntity.group;
    //Remove current file from S3
    await fsController.cloudStorageDelete( module.exports.getFilePath(project, group, currentEntity.metadata.name), "eventhorizonentities");
    //Delete existing entity
    await module.exports.deleteEntity(currentEntity._id);
}

const getEntityByIdProject = async (project, entityId, returnPublic) => {
    let query;
    if (returnPublic) {
        query = {$and: [{_id: mongoose.Types.ObjectId(entityId)}, {"$or": [{"project":project}, {"isPublic": true}]}]};
    } else {
        query = {$and: [{_id: mongoose.Types.ObjectId(entityId)}, {"project":project}]};
    }
    const result = await entityModel.findOne(query);

    return result!==null?result.toObject():null;
}

const getEntitiesOfProject = async (project, returnPublic) => {
    let query;
    if (returnPublic) {
        query = [ {"project":project}, {"isPublic": true} ];
    } else {
        query = {"project":project};
    }
    const result = await entityModel.find(query);

    return result!==null?result:null;
}

const getEntitiesOfProjectWithGroup = async (project, groupID, returnPublic) => {
    let query;
    if (returnPublic) {
        query = { "project":project, "group":groupID, "isPublic": true};
    } else {
        query = { "project":project, "group":groupID };
    }
    const result = await entityModel.find(query);

    return result!==null?result:null;
}

const getEntitiesByProjectGroupTags = async (project, group, tags, fullData, randomElements) => {
    const aggregationQueries = [
        {
            $match: {
                $and: [
                    {"isRestricted": false},
                    {"group": group},
                    {
                        "$or": [
                            {"project":project}, 
                            {"isPublic": true}
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
                    "isPublic": 0,
                    "isRestricted": 0
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

const getEntityDeps = async (project, group, deps) => {
    const aggregationQueries = [
        {
            $match: {
                $and: [
                    {"isRestricted": false},
                    {"group": group},
                    {
                        "$or": [
                            {"project":project}, 
                            {"isPublic": true}
                        ]
                    },
                    {
                        "metadata.hash":  deps
                    }
                ]
            }
        }
    ];

    const result = await asyncModelOperations.aggregate(entityModel, aggregationQueries);

    return result;
}

module.exports = {
    getMetadataFromBody : getMetadataFromBody,
    createEntityFromMetadata : createEntityFromMetadata,
    createEntitiesFromContainer : createEntitiesFromContainer,
    cleanupMetadata : cleanupMetadata,
    getFilePath : getFilePath,
    checkFileExists : checkFileExists,
    createEntity : createEntity,
    updateEntity : updateEntity,
    deleteEntity : deleteEntity,
    deleteEntityComplete : deleteEntityComplete,
    getEntityByIdProject : getEntityByIdProject,
    getEntitiesOfProject : getEntitiesOfProject,
    getEntitiesOfProjectWithGroup : getEntitiesOfProjectWithGroup,
    getEntityDeps : getEntityDeps,
    getEntitiesByProjectGroupTags : getEntitiesByProjectGroupTags
}