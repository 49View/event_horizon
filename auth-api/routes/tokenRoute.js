'use strict';
const express = require("express");
const userController = require("../controllers/userController");
const authController = require("../controllers/authController");
const sessionController = require("../controllers/sessionController");
const logger = require("../logger");
const dataSanitizers = require("../helpers/dataSanitizers");

const router = express.Router();

//
// Clear current token
//
router.put(
    "/cleanToken",
    authController.authenticate,
    async (req, res, next) => {
        try {
            if (req.user === undefined || req.user === null || req.user.hasToken !== true) {
                throw "Unauthorized";
            }
            await sessionController.invalidateSessionById(req.user.sessionId);
            authController.clearCookiesTokenInfo(res);
            res.status(204).send();
        } catch (ex) {
            res.status(401).send(ex);
        }
    }
);

//
// Refresh token using current token credential
//
router.post(
    "/refreshToken",
    authController.authenticate,
    async (req, res, next) => {
        // console.log(req.user);
        try {
            if (req.user === undefined || req.user === null || req.user.hasToken !== true) {
                throw "Unauthorized";
            }
            await sessionController.invalidateSessionById(req.user.sessionId);
            const tokenInfo = await authController.getTokenForUser(req.user, req.ip, req.headers["user-agent"]);
            authController.sendCookiesTokenInfo(res, tokenInfo, true);
        } catch (ex) {
            res.status(401).send(ex);
        }
    }
);

//
// Get token
//
router.post("/getToken", async (req, res, next) => {
    try {
        const paramsDef = [
            { name: "email", type: "email", required: true },
            { name: "password", type: "string", required: true, min: 8 }
        ];
        const params = dataSanitizers.checkBody(req, paramsDef);
        const dbUser = await userController.getUserByEmailPassword(params.email, params.password);
        const tokenInfo = await authController.getTokenForUser(dbUser, req.ip, req.headers["user-agent"]);
        authController.sendCookiesTokenInfo(res, tokenInfo, true);
    } catch (ex) {
        console.log("gettoken failed", ex);
        res.status(401).send(ex);
    }
});

module.exports = router;
