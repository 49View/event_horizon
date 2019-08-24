var fsc = require("../controllers/fsController");
var express = require("express");
var router = express.Router();
const socketController = require("../controllers/socketController");

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
      res.status(201).json({ ETag: data.ETag });
      res.end();
    }
  } catch (ex) {
    console.log("ERROR ADDING FILE TO FS: ", ex);
    res.sendStatus(400);
  }
});

router.post("/entity_to_elaborate/:group/:key", async (req, res, next) => {
  try {
    const data = await fsc.cloudStorageFileUpload(
      req.body,
      req.params.key,
      "eventhorizonfs"
    );
    let json = {
      msg: "cloudStorageFileToElaborate",
      data: {
        name: req.params.key,
        project: req.user.project,
        group: req.params.group,
        uname: req.user.name,
        uemail: req.user.email
      }
    };
    socketController.sendMessageToAllClients(JSON.stringify(json));
    if (res) {
      res.status(201).json({ ETag: data.ETag });
      res.end();
    }
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
