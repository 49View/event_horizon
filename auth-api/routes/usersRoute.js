'use strict';

const express = require("express");
const userController = require("../controllers/userController");
const mailController = require("../controllers/mailController");
const logger = require('../logger');
const authController = require("../controllers/authController");
const dataSanitizers = require("../helpers/dataSanitizers");
const Base64 = require("js-base64").Base64;

const router = express.Router();

const getUser = async req => {
  const result = {
    expires: req.user.expires,
    user: {name: req.user.name, email: req.user.email, guest: req.user.guest},
  };
  if (req.user.hasSession === true) {
    result.session = req.user.sessionId;
  }

  return result;
}

//
// Get current user
// 
router.get("/", authController.authenticate, async (req, res, next) => {
  // console.log("USER GET /");
  res.send(await getUser(req));
});

//
// Create user
//
router.post("/", async (req, res, next) => {
  try {
    const paramsDef = [
      { name: "name", type: "string", required: true},
      { name: "email", type: "email", required: true},
      { name: "password", type: "string", required: true, min: 8}
    ];
    const [params,error] = dataSanitizers.checkBody(req, paramsDef);
    if (error!==null) {
      throw error;
    }
    //logger.info(JSON.stringify(params));
    const user = await userController.createUser(params.name, params.email, params.password);
    if (user===null) {
      throw `email or username already used`;
    }
    res.status(201).send("OK");
  } catch (ex) {
    logger.error(`Error create user: ${ex}`);
    res.status(400).send(`Error creating user: ${ex}`);
  }
});

// //
// // Update user data
// //
// router.put("/:userid", authController.authenticate, async (req, res, next) => {

//   const userid = req.params.userid;
//   const roles = req.body.roles;

//   try {
//     await userController.addRolesForProject(project, email, roles);
//     res.send(await getUser(req));
//   } catch (ex) {
//     res.status(400).send(`Error updating user ${ex.message}`);
//   }
// });

module.exports = router;
