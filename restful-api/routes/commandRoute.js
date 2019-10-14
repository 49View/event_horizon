const express = require("express");
const router = express.Router();
const commandController = require("../controllers/commandController");

router.get("/:commandLine", (req, res, next) => {
  try {
    const cl = decodeURIComponent(req.params.commandLine);
    const ret = commandController.elaborateCommand(cl);
    res.send(ret);
  } catch (ex) {
    console.log("commandLine failed: ", ex);
    res.sendStatus(400);
  }
});

module.exports = router;
