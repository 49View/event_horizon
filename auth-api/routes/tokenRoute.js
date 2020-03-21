'use strict';
const express = require("express");
const userController = require("../controllers/userController");
const authController = require("../controllers/authController");
const sessionController = require("../controllers/sessionController");
const logger = require("../logger");
const globalConfig = require("../config_api");
const dataSanitizers = require("../helpers/dataSanitizers");

const router = express.Router();

const cookieObject = (d, httpOnly) => {
    //console.log("Cloud host:", globalConfig.CloudHost);
    const result = {
        domain: (globalConfig.CloudHost === "localhost") ? globalConfig.CloudHost : `.${globalConfig.CloudHost}`,
        httpOnly: httpOnly,
        sameSite: "Lax",
        signed: true,
        secure: false,
    };
   if (d!==null) { result["expires"]=d; }
   return result;
};

const sendCookieTokenInfo = (res, tokenInfo) => {
    const d = new Date(0);
    d.setUTCSeconds(tokenInfo.expires);
    res.cookie(globalConfig.TokenCookie, tokenInfo.token, cookieObject(d, true));
    res.cookie(globalConfig.AntiForgeryTokenCookie, tokenInfo.antiForgeryToken, cookieObject(d, false));
    res.send(tokenInfo);
}

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
            res.clearCookie(globalConfig.TokenCookie, cookieObject(null, true));
            res.clearCookie(globalConfig.AntiForgeryTokenCookie, cookieObject(null, false));
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
            const tokenInfo = await authController.getToken(
                req.user._id,
                req.ip,
                req.headers["user-agent"] || null
            );
            tokenInfo.user = {
                name: req.user.name,
                email: req.user.email,
                guest: req.user.guest
            };
            sendCookieTokenInfo(res, tokenInfo);
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
        const tokenInfo = await authController.getToken(
            dbUser._id,
            req.ip,
            req.headers["user-agent"] || null
        );
        tokenInfo.user = {
            name: dbUser.name,
            email: dbUser.email,
            guest: dbUser.guest
        };
        sendCookieTokenInfo(res, tokenInfo);
    } catch (ex) {
        console.log("gettoken failed", ex);
        res.status(401).send(ex);
    }
});

module.exports = router;
