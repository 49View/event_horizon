const fsController = require('../controllers/fsController');
const express = require('express');
const router = express.Router();
const entityController = require('../controllers/entityController');

router.get('/content/byId/:id', async (req, res, next) => {
    try {
        const entityId = req.params.id;
        const project = req.user.project;
        //Check existing entity for use project (or public)
        const currentEntity = await entityController.getEntityByIdProject(project, entityId, true);
        if (currentEntity===null) {
            throw "Invalid entity for user project";
        }
        const filePath=entityController.getFilePath(currentEntity.project, currentEntity.group, currentEntity.metadata.content_id);
        const fileData = await fsController.cloudStorageFileGet(filePath, "eventhorizonentities");
        
        fsController.writeFile(res, fileData);

    } catch (ex) {
        console.log("ERROR GETTING ENTITY CONTENT BYID: ", ex);
        res.sendStatus(400);
    }
});

router.get('/content/byGroupTags/:version/:group/:tags', async (req, res, next) => {
    try {
        const group = req.params.group;
        const tags = req.params.tags.split(",");
        const project = req.user.project;
        const version = req.params.version;
        //Check existing entity for use project (or public)
        const foundEntities = await entityController.getEntitiesByProjectGroupTags(project, group, tags, version, true, 1);
        if (foundEntities!==null && foundEntities.length>0) {
            const filePath=entityController.getFilePath(foundEntities[0].project, foundEntities[0].group, foundEntities[0].metadata.content_id);
            const fileData = await fsController.cloudStorageFileGet(filePath, "eventhorizonentities");
            fsController.writeFile(res, fileData);
        } else {
            res.sendStatus(204);
        }
    } catch (ex) {
        console.log("ERROR GETTING ENTITY CONTENT BYGROUPTAGS: ", ex);
        res.sendStatus(400);
    }
});

router.get('/metadata/byGroupTags/:version/:group/:tags', async (req, res, next) => {
    try {
        const group = req.params.group;
        const tags = req.params.tags.split(",");
        const project = req.user.project;
        const version = req.params.version;
        console.log("Entity version: " + version);
        //Check existing entity for use project (or public)
        const foundEntities = await entityController.getEntitiesByProjectGroupTags(project, group, tags, version, false, null);
        if (foundEntities!==null && foundEntities.length>0) {
            res.status(200).send(foundEntities);
        } else {
            res.sendStatus(204);
        }
    } catch (ex) {
        console.log("ERROR GETTING ENTITY METADATA BYGROUPTAGS: ", ex);
        res.sendStatus(400);
    }
});

router.post('/', async (req, res, next) => {

    try {
        const project = req.user.project;
        const metadata = entityController.getMetadataFromBody(true, true, req);
        const { content, group, public, restricted, cleanMetadata } = entityController.cleanupMetadata(metadata);
        const filePath=entityController.getFilePath(project, group, cleanMetadata.content_id);
        //Check content exists in project and group
        const copyEntity = await entityController.checkFileExists(project, group, cleanMetadata.content_id)
        if (copyEntity!==null) {
            //Delete existing entity
            entityController.deleteEntity(copyEntity._id);
        } else {
            //Upload file to S3
            await fsController.cloudStorageFileUpload(content, filePath, "eventhorizonentities");
        }
        //Create entity
        const newEntity = await entityController.createEntity(project, group, public, restricted, cleanMetadata);

        res.status(200).send(newEntity);
    } catch (ex) {
        console.log("ERROR CREATING ENTITY: ", ex);
        res.sendStatus(400);
    }
});

router.put('/:id', async (req, res, next) => {
    try {
        const entityId = req.params.id;
        const project = req.user.project;
        //Check existing entity for use project
        const currentEntity = await entityController.getEntityByIdProject(project, entityId, false);
        if (currentEntity===null) {
            throw "Invalid entity for user project";
        }
        const group = currentEntity.group;
        const metadata = entityController.getMetadataFromBody(false, false, req);
        const { content, public, restricted, cleanMetadata } = entityController.cleanupMetadata(metadata);
        //If content defined
        if (content!==null) {
            //Check content exists in project and group
            const copyEntity = await entityController.checkFileExists(project, group, cleanMetadata.content_id)
            if (copyEntity!==null && !copyEntity._id.equals(currentEntity._id)) {
                throw "Content already defined";
            } else if (copyEntity===null) {
                //Remove current file from S3
                await fsController.cloudStorageDelete(entityController.getFilePath(project, group, currentEntity.metadata.content_id), "eventhorizonentities");
                //Upload file to S3
                const filePath=entityController.getFilePath(project, group, cleanMetadata.content_id);
                await fsController.cloudStorageFileUpload(content, filePath, "eventhorizonentities");
            }
        } else {
            //If content don't change use current content_id 
            cleanMetadata.content_id=currentEntity.metadata.content_id;
        }
        //Update entity
        await entityController.updateEntity(currentEntity._id, project, group, public, restricted, cleanMetadata);

        res.status(204).send();
    } catch (ex) {
        console.log("ERROR UPDATING ENTITY: ", ex);
        res.sendStatus(400);
    }

});

router.delete('/:id', async (req, res, next) => {
    try {
        const entityId = req.params.id;
        const project = req.user.project;
        //Check existing entity for use project
        const currentEntity = await entityController.getEntityByIdProject(project, entityId, false);
        if (currentEntity===null) {
            console.log( "[INFO] Deletion of " + entityId + " not found, no operations performed" );
            res.status(204).send();
        } else {
            await entityController.deleteEntityComplete( project, currentEntity);
            res.status(201).send();
        }
    } catch (ex) {
        console.log("ERROR DELETING ENTITY: ", ex);
        res.sendStatus(400);
    }

});

router.delete('/', async (req, res, next) => {
    try {
        const project = req.user.project;
        const entities =await entityController.getEntitiesOfProject( project );
        if (entities===null) {
            res.status(204).send();
        } else {
            for ( const entity of entities ) {
                await entityController.deleteEntityComplete( project, entity );
            }
            res.status(201).send();
        }
    } catch (ex) {
        console.log("ERROR DELETING ENTITIES: ", ex);
        res.sendStatus(400);
    }

});

module.exports = router;
