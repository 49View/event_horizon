const fsc = require('../controllers/fsController');
const path = require('path');
const express = require('express');
const mongoose = require('mongoose');
const router = express.Router();
const entityModel = require('../models/entity');
const entityKeyModel = require('../models/entity_key');
const asyncModelOperations = require('../assistants/asyncModelOperations');

const sendFile = ( res, data ) => {
    res.writeHead(200, {
        'Content-Type': data.ContentType,
        'Content-Last-Modified': data.LastModified,
        'ETag': data.ETag,
        'Content-Length': data.ContentLength
    });
    res.end(data["Body"]);
}

const getFilePath = (group, filename) => {
    return "entities/"+group+"/"+filename;
}

// const checkFileExists = async (group, filename) => {

//     let result = null;
//     try {
//         const query = {$and: [{"metadata.file":filename}, {"group":group}]};
//         //const query = { "metadata.file": key};
//         console.log(query);
//         result=await entityModel.findOne(query);
//         console.log(result);
//     } catch (ex) {
//         console.log("checkFileExists", ex);
//         result=null;
//     }
//     return result;
// }

const checkNameExists = async (group, name) => {

    let result = null;
    try {
        const query = {$and: [{"metadata.keyname":name}, {"group":group}]};
        result=await entityModel.findOne(query);
    } catch (ex) {
        result=null;
    }
    return result;
}

const getKeysFromName = (group, name) => {

    const keysInNames = name.split(/[\s_\.]+/).map(k => k.toLowerCase());
    const keys = new Set(keysInNames);
    if (group!==null) {
        keys.add("group_"+group);
    }

    console.log("KEYS: ",keys);
    return keys;
}

const getKeysFromMetadata = (group, metadata) => {
    const entityKeys = new Set(metadata.tags);        
    if (group!==null) {
        entityKeys.add("group_"+group);
    }

    return entityKeys
}

// const getKeysFromFilename = (group, filename) => {

//     const keys = path.parse(filename).name.split(/[\s_\.]+/).map(k => k.toLowerCase());
//     keys.push("group_"+group);

//     console.log("KEYS: ",keys);
//     return keys;
// }

// const createEntityForFile = async (group, filename) => {
//     const newEntityDB = new entityModel({ group: group, metadata: {file: filename, name: path.parse(filename).name}});
//     await newEntityDB.save();
//     return newEntityDB.toObject();
// }


// const createEntityForNoFile = async (group, name, metadata) => {
//     const entityMetaData = { ...metadata
//         , name: name
//     };
//     const newEntityDB = new entityModel({ group: group, metadata: entityMetaData});
//     await newEntityDB.save();
//     return newEntityDB.toObject();
// }

const createEntityKeysRelations = async(entityId,keys) => {

    const upsertOps = [];
    keys.forEach(key => {
        upsertOps.push(
            {
                updateOne: {
                    filter: { key: key},
                    update: {
                        "$addToSet": {"entities": mongoose.Types.ObjectId(entityId)},
                        "$setOnInsert": { "key": key}
                    },
                    upsert: true
                }
            }
        );
    });
    console.log("UPSERT OPS:", JSON.stringify(upsertOps));
    let result=null;
    try {
        result=await entityKeyModel.bulkWrite(upsertOps, {ordered: false});
    } catch (ex) {
        console.log("ERROR:",ex);
        //console.log(ex.writeErrors[0].err);
        result=null;
    }
    return result;
}

const deleteEntity = async (entityId) => {
    let result=null;
    try {
        result=await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId)});
        console.log("RESULT: ", result.data);
    } catch (ex) {
        console.log("ERROR:",ex);
        //console.log(ex.writeErrors[0].err);
        throw ex;
    }
    return result;
}

const deleteEntityKeys = async (entityId) => {
    const updateOps = [
        {
            updateMany: {
                filter: { entities: { $elemMatch: { $eq: mongoose.Types.ObjectId(entityId) }}},
                update: {
                    $pull: { entities: mongoose.Types.ObjectId(entityId)}
                }
            }
        },
        {   
            deleteMany: {
                filter: { entities: { $size: 0 }}
            }
        }
    ];
    let result=null;
    try {
        result=await entityKeyModel.bulkWrite(updateOps, {ordered: true});
        console.log("RESULT: ", result.data);
    } catch (ex) {
        console.log("ERROR:",ex);
        //console.log(ex.writeErrors[0].err);
        throw ex;
    }
    return result;
}

const updateEntity = async (filterQuery, updateQuery) => {

    let result=null;
    try{
        result=await entityModel.updateOne(filterQuery, updateQuery);
        console.log(result);        
    } catch (ex) {
        console.log("ERROR:",ex);
        throw ex;
    }

    return result;
}

const addMetadataToEntityById = async (entityId, metadata) => {

    const newMetadata = {};

    for (let m in metadata) {
        newMetadata["metadata."+m]=metadata[m];
    }
    const filterQuery = { _id: mongoose.Types.ObjectId(entityId) };
    const updateQuery = { $set: newMetadata };

    return await updateEntity(filterQuery, updateQuery);
}

const addMetadataToEntityByGroupAndName = async (group, name, metadata) => {

    const newMetadata = {};

    for (let m in metadata) {
        newMetadata["metadata."+m]=metadata[m];
    }
    const filterQuery = { group: group, "metadata.keyname": name };
    const updateQuery = { $set: newMetadata };

    return await updateEntity(filterQuery, updateQuery);
}

const addMetadataToEntityByGroupAndFilename = async (group, filename, metadata) => {

    const newMetadata = {};

    for (let m in metadata) {
        newMetadata["metadata."+m]=metadata[m];
    }
    const filterQuery = { group: group, "metadata.file": filename };
    const updateQuery = { $set: newMetadata };

    return await updateEntity(filterQuery, updateQuery);
}

const prepareAggregationQuery = (keys) => {

    return [
        { $match: { 
            key: {$in: keys } 
        }},
        { $group: {
            _id: 0,
            "sets": {
                $push: "$entities"
            },
            "initialSet": {
                $first: "$entities"
            }
        }},
        { $project: {
            "result": {
                $reduce: {
                    input: "$sets",
                    initialValue: { $cond: { if: { $eq: [{$size:"$sets"}, keys.length]}, then: "$initialSet", else:[] }},
                    in: { "$setIntersection": [ "$$value", "$$this"] }
                }
            }            
        }},
        { $unwind: {
                path: "$result"
        }}
    ]

}

const getEntities = async (req, res, randomElementNumber) => {
    //Prepare keys
    const keys = req.params.keys.split(",");
    keys.push("group_"+req.params.group);

    console.log( keys );

    const aggregationQuery = prepareAggregationQuery(keys);
    if (randomElementNumber!==null) {
        aggregationQuery.push(
            { $sample: {
                size: randomElementNumber
            }}
        );

    }
    aggregationQuery.push(
        { $lookup: {
            from: "entities",
            localField: "result",
            foreignField: "_id",
            as: "entities"
        }}
    );
    aggregationQuery.push(
        { $project: {
            _id: 0,
            entity: { $arrayElemAt: ["$entities",0] }
        }}
    );
    // aggregationQuery.push(
    //     { $unwind: {
    //         path: "$entity"
    //     }}
    // );
    aggregationQuery.push(
        { $replaceRoot: { 
            newRoot: "$entity.metadata" 
        }}
    );

    return await asyncModelOperations.aggregate(entityKeyModel, aggregationQuery);
    
}

const deleteEntityWithDependencies = async (entity ) => {
    let result = { entity: null, entityKeys: null, error : null};

    if (entity===null) {
        throw "Entity doesn't exist";
    }
    if (entity.metadata.file!==null) {
        const path = getFilePath(entity.group, entity.metadata.file);
        await fsc.cloudStorageDelete(path);
    }
    result.entity=await deleteEntity(entity._id);
    result.entityKeys = await deleteEntityKeys(entity._id);

    return result;
}

const deleteListWithDependencies = async (entities) => {
    let result = { entity: null, entityKeys: null, error : null};
    if (entities===null) {
        throw "Entity doesn't exist";
    }

    try {
        let resArray = [];
        for ( eo of entities ) {
            resArray.push( await deleteEntityWithDependencies( eo.toObject() ) );
        }
        return resArray;
    }
    catch (ex) {
        result.error=ex;
    }

    return result;
}

const deleteId = async (eId) => {

    let entity = {};

    entity = await entityModel.findOne({ _id: mongoose.Types.ObjectId(eId)});

    if (entity!==null) {
        entity=entity.toObject();
    }

    try {
        return await deleteEntityWithDependencies( entity );
    }
    catch (ex) {
        result.error=ex;
    }

    return result;
}

const deleteGroup = async (eGroup) => {
    let entities = [];
    entities = await entityModel.find({ group : eGroup });
    return await deleteListWithDependencies(entities);
}

const deleteEntities = async () => {
    let entities = [];
    entities = await entityModel.find();
    return await deleteListWithDependencies(entities);
}

router.get('/entities/one/:group/:keys', async (req, res, next) => {

    let result = null;
    let error = null;
    try {
        result=await getEntities(req, res, 1);
    } catch (ex)
    {
        error=ex;
    }
    if (error!==null) {
        res.status(500).send(error);
    } else {
        res.send(result);
    }
});

router.get('/entities/all/:group/:keys', async (req, res, next) => {

    let result = null;
    let error = null;
    try {
        result=await getEntities(req, res, null);
    } catch (ex)
    {
        error=ex;
    }
    if (error!==null) {
        res.status(500).send(error);
    } else {
        res.send(result);
    }
});

router.get('/entities/onebinary/:group/:keys', async (req, res, next) => {

    let result = null;
    let error = null;
    let binaryContent = null;
    try {
        result=await getEntities(req, res, 1);
        if (result && result.length>0) {
            console.log(result);
            const entityMetadata = result[0];
            console.log(entityMetadata.file);
            if (entityMetadata.file) {
                let path=getFilePath(req.params.group, entityMetadata.file);
                binaryContent = await fsc.cloudStorageFileGet(path);
            }
        }
    } catch (ex)
    {
        console.log(ex);
        error=ex;
    }
    if (error!==null) {
        res.status(500).send(error);
    } else {
        if (binaryContent!==null) {
            sendFile(res, binaryContent);
        } else {
            res.status(204).send();
        }
    }
});


router.post('/entities/:group/:filename', async (req, res, next) => {

    let error=null;
    try {
        let group =req.params.group;
        let filename=req.params.filename;
        let keyname=path.parse(filename).name;

        console.log(group+","+filename+","+keyname);

        //Check name exists
        let entity = await checkNameExists(group,keyname);
        if (entity!==null) {
            await deleteId( entity.id );
        }
        const metadata = req.body;
        if (!('raw' in metadata)) {
            throw "Metadata doesn't contain file content";
        }
        if (!('tags' in metadata)) {
            throw "Metadata doesn't contain entity's tags";
        }

        //Parse file content
        const fileContent = new Buffer(metadata.raw, "base64");
        //Prepare metadata for upload
        delete metadata.raw;
        metadata.keyname=keyname;
        metadata.file=filename;

        //Get keys from name and tags
        const entityKeys = getKeysFromMetadata(group, metadata);        
    
        //Upload file
        const filenamePath=getFilePath(group, filename);
        const uploadResult=await fsc.cloudStorageFileUpdate(fileContent, filenamePath);

        //Create entity
        const newEntityDB = new entityModel({ group: group, metadata: metadata});
        await newEntityDB.save();
        const newEntity=newEntityDB.toObject();

        //Create entity keys relations
        await createEntityKeysRelations(newEntity._id.toString(), entityKeys);

        res.send({ entity: newEntity, keys: entityKeys, upload: uploadResult});
    } catch (ex) {
        console.log(ex);
        res.status(500).send(`Entity ${keyname} could not be created.`+ex);
    }
});



// router.post('/entities/nofile/:group/:name', async (req, res, next) => {

//     let entity = await checkNameExists(req.params.group,req.params.name);

//     if (entity!==null) {
//         res.status(500).send(`Entity with name ${req.params.name} exists`);
//         return;
//     }
//     const entityKeys = getKeysFromName(req.params.group, req.params.name);
//     try {
//         //Create entity
//         const newEntity = await createEntityForNoFile(req.params.group, req.params.name, req.body);

//         //Create entity keys relations
//         createEntityKeysRelations(newEntity._id.toString(), entityKeys);

//         res.send({ entity: newEntity, keys: entityKeys});
//     } catch (ex) {
//         console.log(ex);
//         res.status(500).send(`Entity ${req.params.name} could not be created`)
//     }
// });

// router.post('/entities/withfile/:group/:filename', async (req, res, next) => {

//     let entity = await checkFileExists(req.params.group,req.params.filename);

//     if (entity!==null) {
//         res.status(500).send(`Entity with filename ${req.params.filename} exists`);
//         return;
//     }

//     //Build file destination path
//     const path = getFilePath(req.params.group, req.params.filename);
//     // const path = "entities/"+req.params.group+"/"+req.params.filename;
//     //Get entity keys from file name
//     const entityKeys = getKeysFromFilename(req.params.group, req.params.filename);
//     try {
//         //Uplaod file
//         const uploadData = await fsc.cloudStorageFileUpdate(req.body, path);
//         //Create entity
//         const newEntity = await createEntityForFile(req.params.group, req.params.filename);
//         //Create entity keys relations
//         createEntityKeysRelations(newEntity._id.toString(), entityKeys);

//         res.send({ entity: newEntity, keys: entityKeys});

//     } catch (ex) {
//         console.log(ex);
//         res.status(500).send(`File ${req.params.filename} could not be uploaded`)
//     }
// });

router.put('/entities/addMetadata/byId/:entityId', async (req, res, next) => {

    let result=null;
    let error=null;
    
    try {
        result=addMetadataToEntityById(req.params.entityId, req.body);
    } catch (ex) {
        error=ex;
    }

    if (error) {
        res.status(500).send(error);
    } else {
        res.send(result);
    }
});

router.put('/entities/addMetadata/byGroupAndName/:group/:keyname', async (req, res, next) => {

    let result=null;
    let error=null;
    
    try {
        result=addMetadataToEntityByGroupAndName(req.params.group, req.params.keyname, req.body);
    } catch (ex) {
        error=ex;
    }

    if (error) {
        res.status(500).send(error);
    } else {
        res.send(result);
    }
});

router.put('/entities/addMetadata/byGroupAndFilename/:group/:filename', async (req, res, next) => {

    let result=null;
    let error=null;
    
    try {
        result=addMetadataToEntityByGroupAndFilename(req.params.group, req.params.filename, req.body);
    } catch (ex) {
        error=ex;
    }

    if (error) {
        res.status(500).send(error);
    } else {
        res.send(result);
    }
});

router.put('/entities/addKeys/:entityId/:keys', async (req, res, next) => {

    const result={ error: true};
    
    const entityExist = await entityModel.findOne({ _id: mongoose.Types.ObjectId(req.params.entityId)});
    console.log(entityExist);
    if (entityExist!==null) {
        result.error=false;
        const entityKeys = getKeysFromName(null, req.params.keys);
        result.error=result.error || await createEntityKeysRelations(req.params.entityId, entityKeys);
    }
    res.send(result);
});

router.delete('/entities/byId/:entityId', async (req, res, next) => {

    const result = await deleteId(req.params.entityId);

    if (result.error) {
        res.status(500).send(result.error);
    } else {
        res.send(result);
    }
});

router.delete('/entities/byGroup/:group/:secret', async (req, res, next) => {

    if ( req.params.secret != "ucarcamagnu" ) {
        res.status(500).send("You are not cool enough to delete a group");
        return;
    }

    const result = await deleteGroup(req.params.group);

    if (result.error) {
        res.status(500).send(result.error);
    } else {
        res.send(result);
    }
});

router.delete('/entities/:secret/:forreal', async (req, res, next) => {

    if ( req.params.secret != "ucarcamagnu" || req.params.forreal != "forreal" ) {
        res.status(500).send("You are not cool enough to delete everything");
        return;
    }

    const result = await deleteEntities(req.params.group);

    if (result.error) {
        res.status(500).send(result.error);
    } else {
        res.send(result);
    }
});

module.exports = router;
