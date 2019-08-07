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

module.exports = router;
