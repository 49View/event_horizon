var fsc = require("../controllers/fsController");
var express = require("express");
var router = express.Router();
const socketController = require("../controllers/socketController");
const entityController = require("../controllers/entityController");
const sharp = require("sharp");
const db = require('../db');

router.get("/:key", async (req, res, next) => {
    try {
        const data = await fsc.cloudStorageFileGet(
            req.params.key,
            "eventhorizonfs"
        );
        fsc.writeFile(res, data);
    } catch (ex) {
        console.log("ERROR GETTING FILE FROM FS: ", ex);
        res.sendStatus(400);
    }
});

router.post("/resizeimage/:width/:height/:key", async (req, res, next) => {
    try {
        const fullSizeImage = Buffer.from(req.body);
        const scaledDown = await sharp(fullSizeImage)
            .resize(Number(req.params.width), Number(req.params.height), {
                fit: "inside",
                withoutEnlargement: true
            })
            .toFormat("jpg")
            .toBuffer();

        const data = await fsc.cloudStorageFileUpload(
            scaledDown,
            req.params.key,
            "eventhorizonfs"
        );
        let json = {
            msg: "cloudStorageFileAdded",
            data: {
                name: req.params.key,
                project: req.user.project
            }
        };
        socketController.sendMessageToAllClients(JSON.stringify(json));
        if (res) {
            res
                .status(201)
                .json({ETag: data.ETag, buffer: scaledDown.toString("base64")});
            res.end();
        }
    } catch (ex) {
        console.log("ERROR RESIZING AND ADDING IMAGE FILE TO FS: ", ex);
        res.sendStatus(400);
    }
});

router.post("/:key", async (req, res, next) => {
    try {
        const data = await fsc.cloudStorageFileUpload(
            req.body,
            req.params.key,
            "eventhorizonfs"
        );
        let json = {
            msg: "cloudStorageFileAdded",
            data: {
                name: req.params.key,
                project: req.user.project
            }
        };
        socketController.sendMessageToAllClients(JSON.stringify(json));
        if (res) {
            res.status(201).json({ETag: data.ETag});
            res.end();
        }
    } catch (ex) {
        console.log("ERROR ADDING FILE TO FS: ", ex);
        res.sendStatus(400);
    }
});

router.post("/entity_to_elaborate/:group/:key(*)", async (req, res, next) => {
    try {
        const filename = req.params.key;
        const metadataComp = {
            "metadata.group": req.params.group,
            "metadata.project": req.user.project
        }
        const metadata = {
            group: req.params.group,
            project: req.user.project,
            username: req.user.name,
            useremail: req.user.email,
        }
        const bInserted = await db.fsUpsert( db.bucketSourceAssets, filename, req.body, metadata, metadataComp, entityController.checkEntityExistsByFSId );
        res.sendStatus( bInserted ? 200 : 204 );
    } catch (ex) {
        console.log("ERROR ADDING FILE TO FS: ", ex);
        res.sendStatus(400);
    }
});

router.delete("/:key", async (req, res, next) => {
    try {
        const result = await fsc.cloudStorageDelete(
            req.params.key,
            "eventhorizonfs"
        );
        res.sendStatus(204);
    } catch (ex) {
        console.log("ERROR DELETING FILE IN FS: ", ex);
        res.sendStatus(400);
    }
});

module.exports = router;
