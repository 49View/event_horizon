'use strict';

const express = require("express");
const userController = require("../controllers/userController");
const mailController = require("../controllers/mailController");
const logger = require('../logger');
const authController = require("../controllers/authController");
const dataSanitizers = require("../helpers/dataSanitizers");

const router = express.Router();

const getUser = async req => {
    return {
        expires: req.user.expires,
        user: { name: req.user.name, email: req.user.email, guest: req.user.guest },
        session: req.user.hasSession ? req.user.sessionId : undefined
    };
}

//
// Get current user
// 
router.get("/", authController.authenticate, async (req, res, next) => {
    res.send(await getUser(req));
});

//
// Create user
//
router.post("/", async (req, res, next) => {
    try {
        const paramsDef = [
            { name: "name", type: "string", required: true },
            { name: "email", type: "email", required: true },
            { name: "password", type: "string", required: true, min: 8 }
        ];
        const params = dataSanitizers.checkBody(req, paramsDef);
        const user = await userController.createUser(params.name, params.email, params.password);
        const tokenInfo = await authController.getTokenForUser(user, req.ip, req.headers["user-agent"]);
        authController.sendCookiesTokenInfo(res, tokenInfo, true);
    } catch (ex) {
        logger.error(`Error create user: ${ex}`);
        res.status(400).send(`Error creating user: ${ex}`);
    }
});


module.exports = router;
