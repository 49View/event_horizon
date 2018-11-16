const fsc = require('../controllers/fsController');
const path = require('path');
const express = require('express');
const mongoose = require('mongoose');
const crypto = require('crypto');
const router = express.Router();
const entityModel = require('../models/entity');
const entityController = require('../controllers/entityController');

router.post('/', async (req, res, next) => {

    try {
        const metadata = entityController.getMetadataFromBody(true, true, req);
        const { content, group, keys, cleanMetadata } = entityController.cleanupMetadata(null, metadata);

        console.log("CONTENT:", content);
        console.log("GROUP:", group);
        console.log("KEYS:", keys);

        res.status(200).send(cleanMetadata);
    } catch (ex) {
        console.log("ERROR CREATING ENTITY: ", ex);
        res.sendStatus(400);
    }
});

router.put('/:id', async (req, res, next) => {
    try {
        //Get entity from DB by id
        const entityDB = { contentHash: "a123b456"};

        const metadata = entityController.getMetadataFromBody(false, false, req);
        const { keys, cleanMetadata } = entityController.cleanupMetadata(entityDB.contentHash, metadata);

        console.log("KEYS:", keys);

        res.status(200).send(cleanMetadata);
    } catch (ex) {
        console.log("ERROR UPDATING ENTITY: ", ex);
        res.sendStatus(400);
    }

});


// router.get('/entities/one/:group/:keys', async (req, res, next) => {

//     let result = null;
//     let error = null;
//     try {
//         result=await entityController.getEntities(req, res, 1);
//     } catch (ex)
//     {
//         error=ex;
//     }
//     if (error!==null) {
//         res.status(500).send(error);
//     } else {
//         res.send(result);
//     }
// });

// router.get('/entities/all/:group/:keys', async (req, res, next) => {

//     console.log("/entities/all", req.route);
//     let result = null;
//     let error = null;
//     try {
//         result=await entityController.getEntities(req, res, null);
//     } catch (ex)
//     {
//         error=ex;
//     }
//     if (error!==null) {
//         res.status(500).send(error);
//     } else {
//         res.send(result);
//     }
// });

// router.get('/entities/onebinary/:group/:keys', async (req, res, next) => {

//     let result = null;
//     let error = null;
//     let binaryContent = null;
//     try {
//         result=await entityController.getEntities(req, res, 1);
//         if (result && result.length>0) {
//             console.log(result);
//             const entityMetadata = result[0];
//             console.log(entityMetadata.file);
//             if (entityMetadata.file) {
//                 let path=entityController.getFilePath(req.params.group, entityMetadata.file);
//                 binaryContent = await fsc.cloudStorageFileGet(path);
//             }
//         }
//     } catch (ex)
//     {
//         console.log(ex);
//         error=ex;
//     }
//     if (error!==null) {
//         res.status(500).send(error);
//     } else {
//         if (binaryContent!==null) {
//             sendFile(res, binaryContent);
//         } else {
//             res.status(204).send();
//         }
//     }
// });


// router.post('/entities/:group/:filename', async (req, res, next) => {

//     let error=null;
//     try {
//         let group =req.params.group;
//         let filename=req.params.filename;
//         let keyname=path.parse(filename).name;

//         console.log(group+","+filename+","+keyname);

//         //Check name exists
//         let entity = await entityController.checkNameExists(group,keyname);
//         if (entity!==null) {
//             await deleteId( entity.id );
//         }
//         const metadata = req.body;
//         if (!('raw' in metadata)) {
//             throw "Metadata doesn't contain file content";
//         }

//         //Parse file content
//         const fileContent = new Buffer(metadata.raw, "base64");
//         //Prepare metadata for upload
//         delete metadata.raw;
//         metadata.keyname=keyname;
//         metadata.file=filename;

//         //Get keys from name
//         const entityKeys = entityController.getKeysFromName(group, keyname);        

//         //Upload file
//         const filenamePath=entityController.getFilePath(group, filename);
//         const uploadResult=await fsc.cloudStorageFileUpdate(fileContent, filenamePath);

//         //Create entity
//         const newEntityDB = new entityModel({ group: group, metadata: metadata});
//         await newEntityDB.save();
//         const newEntity=newEntityDB.toObject();

//         //Create entity keys relations
//         await entityController.createEntityKeysRelations(newEntity._id.toString(), entityKeys);

//         res.send({ entity: newEntity, keys: entityKeys, upload: uploadResult});
//     } catch (ex) {
//         console.log(ex);
//         res.status(500).send(`Entity ${keyname} could not be created.`+ex);
//     }
// });

// router.put('/entities/addMetadata/byId/:entityId', async (req, res, next) => {

//     let result=null;
//     let error=null;
    
//     try {
//         result=entityController.addMetadataToEntityById(req.params.entityId, req.body);
//     } catch (ex) {
//         error=ex;
//     }

//     if (error) {
//         res.status(500).send(error);
//     } else {
//         res.send(result);
//     }
// });

// router.put('/entities/addMetadata/byGroupAndName/:group/:keyname', async (req, res, next) => {

//     let result=null;
//     let error=null;
    
//     try {
//         result=entityController.addMetadataToEntityByGroupAndName(req.params.group, req.params.keyname, req.body);
//     } catch (ex) {
//         error=ex;
//     }

//     if (error) {
//         res.status(500).send(error);
//     } else {
//         res.send(result);
//     }
// });

// router.put('/entities/addMetadata/byGroupAndFilename/:group/:filename', async (req, res, next) => {

//     let result=null;
//     let error=null;
    
//     try {
//         result=entityController.addMetadataToEntityByGroupAndFilename(req.params.group, req.params.filename, req.body);
//     } catch (ex) {
//         error=ex;
//     }

//     if (error) {
//         res.status(500).send(error);
//     } else {
//         res.send(result);
//     }
// });

// router.put('/entities/addKeys/:entityId/:keys', async (req, res, next) => {

//     const result={ error: true};
    
//     const entityExist = await entityModel.findOne({ _id: mongoose.Types.ObjectId(req.params.entityId)});
//     console.log(entityExist);
//     if (entityExist!==null) {
//         result.error=false;
//         const entityKeys = entityController.getKeysFromName(null, req.params.keys);
//         result.error=result.error || await entityController.createEntityKeysRelations(req.params.entityId, entityKeys);
//     }
//     res.send(result);
// });

// router.delete('/entities/byId/:entityId', async (req, res, next) => {

//     const result = await entityController.deleteId(req.params.entityId);

//     if (result.error) {
//         res.status(500).send(result.error);
//     } else {
//         res.send(result);
//     }
// });

module.exports = router;
