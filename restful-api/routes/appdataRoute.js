const express = require("express");
const router = express.Router();
const appdataController = require("../controllers/appdataController");

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
    let content = {
      ...req.body,
      project: req.user.project
    };
    const ret = await appdataController.addApp(content);
    res.json(ret);
  } catch (ex) {
    console.log("Error creating app", ex);
    res.sendStatus(400);
  }
});

module.exports = router;
