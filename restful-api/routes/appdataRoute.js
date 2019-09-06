const express = require("express");
const router = express.Router();
const appdataController = require("../controllers/appdataController");
const metadataAssistant = require("../assistants/metadataAssistant");

router.get("/list/:project", async (req, res, next) => {
  try {
    const ret = await appdataController.listAppsForProject(req.params.project);
    res.json(ret);
  } catch (ex) {
    console.log("Error getting app", ex);
    res.sendStatus(400);
  }
});

router.get("/convert/:key", async (req, res, next) => {
  try {
    let ret = await appdataController.getApp(req.params.key);
    if (!ret.entities) ret.entities = [];
    ret.entities.push({
      key: "geoms",
      value: ret.geoms
    });
    ret.entities.push({
      key: "colors",
      value: ret.colors
    });
    ret.entities.push({
      key: "materials",
      value: ret.materials
    });
    ret.entities.push({
      key: "profiles",
      value: ret.profiles
    });
    ret.entities.push({
      key: "images",
      value: ret.images
    });
    ret.entities.push({
      key: "fonts",
      value: ret.fonts
    });
    ret.entities.push({
      key: "scripts",
      value: ret.scripts ? ret.scripts : []
    });
    // delete ret._id;
    delete ret.geoms;
    delete ret.colors;
    delete ret.materials;
    delete ret.profiles;
    delete ret.images;
    delete ret.fonts;
    delete ret.scripts;
    const newRet = await appdataController.replaceApp(ret);
    res.json(newRet);
  } catch (ex) {
    console.log("Error getting app", ex);
    res.sendStatus(400);
  }
});

router.get("/:key", async (req, res, next) => {
  try {
    const ret = await appdataController.getApp(req.params.key);
    res.json(ret);
  } catch (ex) {
    console.log("Error getting app", ex);
    res.sendStatus(400);
  }
});

router.post("/", async (req, res, next) => {
  //   console.log("Create app ", req.body);
  try {
    const content = {
      ...req.body,
      group: "app",
      project: req.user.project
    };
    metadataAssistant.udpateMetadata(content.metadata, content);
    const ret = await appdataController.addApp(content);
    res.json(ret);
  } catch (ex) {
    console.log("Error creating app", ex);
    res.sendStatus(400);
  }
});

// Update
router.put("/:key", async (req, res, next) => {
  try {
    const content = await appdataController.getApp(req.params.key);
    if (!content.metadata) {
      content.metadata = {
        name: content.mKey
      };
    }
    metadataAssistant.udpateMetadata(content.metadata, content);
    const ret = await appdataController.updateApp(content);
    res.json(ret);
  } catch (ex) {
    console.log("Error creating app", ex);
    res.sendStatus(400);
  }
});

router.put("/:key/:group/:value", async (req, res, next) => {
  try {
    const content = await appdataController.getApp(req.params.key);
    if (content === null) {
      throw ("appData Key not found", req.params.key);
    }
    let bAssigned = false;
    for (let entI = 0; entI < content.entities.length; entI++) {
      if (content.entities[entI].key === req.params.group) {
        if (content.entities[entI].value.indexOf(req.params.value) === -1) {
          content.entities[entI].value.push(req.params.value);
          bAssigned = true;
          break;
        } else {
          res.json(content);
          return;
        }
      }
    }
    if (!bAssigned) {
      content.entities.push({
        key: req.params.group,
        value: [req.params.value]
      });
    }
    const ret = await appdataController.updateApp(content);
    res.json(ret);
  } catch (ex) {
    console.log("Error creating app", ex);
    res.sendStatus(400);
  }
});

module.exports = router;
