const express = require("express");
const router = express.Router();
const socketController = require("../controllers/socketController");

router.put("/", (req, res, next) => {
  let json = { msg: "hello", name: "vitra", project: req.user.project };
  socketController.sendMessageToAllClients(JSON.stringify(json));

  res.send(json);
});

module.exports = router;
