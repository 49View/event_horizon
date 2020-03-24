'use strict';
const passport = require("passport");
const CustomStrategy = require("passport-custom").Strategy;
const globalConfig = require("../config_api.js");
const jsonWebToken = require("jsonwebtoken");
const userController = require("./userController");
const routeAuthorizationModel = require("../models/route_authorization");
const sessionController = require("../controllers/sessionController");
const cryptoController = require("../controllers/cryptoController");
const logger = require('../logger');


const jwtOptions = {
    issuer: "ateventhorizon.com",
    algorithm: "HS384"
};

const getUserFromRequest = async req => {
    //Get jwt token from signed cookies and anti forgery token from headers
    let jwtToken = null;
    if (req && req.signedCookies && req.signedCookies[globalConfig.TokenCookie]) {
        jwtToken = req.signedCookies[globalConfig.TokenCookie];
    }
    let aftToken = null;
    if (req && req.headers && req.headers[globalConfig.AntiForgeryTokenCookie]) {
        aftToken = req.headers[globalConfig.AntiForgeryTokenCookie];
    }
    if (jwtToken === null || aftToken === null) {
        throw "Invalid tokens";
    }
    // Extract token payload
    const jwtPayload = jsonWebToken.verify(
        jwtToken,
        globalConfig.JWTSecret,
        jwtOptions
    );
    //logger.info("JWT PAYLOAD", jwtPayload);
    const sessionId = jwtPayload.sub;
    const session = await sessionController.getValidSessionById(sessionId);
    if (session === null) {
        throw `Invalid session ${sessionId}`;
    }
    //Check antiforgery token
    if (aftToken !== session.antiForgeryToken) {
        throw `Invalid antiforgery token for ${sessionId}`;
    } 
    //Get session user
    const user = await userController.getUserById(session.userId);
    if (user === null) {
        throw "Invalid user";
    } 
    user.roles = user.roles.map(v => v.toLowerCase());
    user.project = session.project;
    user.sessionId = sessionId;
    user.expires = jwtPayload.exp;
    user.hasToken = true;
    user.hasSession = true;
    return user;
}

const initializeAuthentication = () => {

    passport.use("cookie-antiforgery",
        new CustomStrategy(async (req, done) => {

            try {
                //logger.info("Anti forgery token strategy");
                const user = await getUserFromRequest(req);
                done(null, user);
            } catch (ex) {
                logger.error(`Error in token: ${ex}`);
                done(null, false, { message: ex });
            }
        })
    );

};

const createJwtToken = async (sessionId, issuedAt, expiresAt) => {
    const payload = {
        iat: issuedAt,
        exp: expiresAt,
        sub: sessionId
    };
    //logger.info(globalConfig.JWTSecret);
    const jwt = await jsonWebToken.sign(
        payload,
        globalConfig.JWTSecret,
        jwtOptions
    );

    return jwt;
};

const getToken = async (userId, ipAddress, userAgent) => {
    const issuedAt = Math.floor(Date.now() / 1000);
    const expiresAt = issuedAt + globalConfig.JwtExpiresSeconds;
    const session = await sessionController.createSession(
        userId,
        cryptoController.generateId("AntiForgeryToken"),
        ipAddress,
        userAgent,
        issuedAt,
        expiresAt
    );
    return {
        session: session.ids,
        antiForgeryToken: session.antiForgeryToken,
        token: await createJwtToken(session.ids, issuedAt, expiresAt),
        expires: expiresAt
    };
};

const verifyToken = async jwtToken => {
    return await jsonWebToken.verify(
        jwtToken,
        globalConfig.JWTSecret,
        jwtOptions
    );
};

const getTokenForUser = async (user, ipAddress, userAgent) => {
    const tokenInfo = await getToken(user._id, ipAddress, userAgent || null);
    tokenInfo.user = {
        name: user.name,
        email: user.email,
        guest: user.guest
    };
    return tokenInfo;
}

const cookieObject = (d, httpOnly) => {
    //console.log("Cloud host:", globalConfig.CloudHost);
    const result = {
        domain: (globalConfig.CloudHost === "localhost") ? globalConfig.CloudHost : `.${globalConfig.CloudHost}`,
        httpOnly: httpOnly,
        sameSite: "Lax",
        signed: true,
        secure: true,
    };
   if (d!==null) { result["expires"]=d; }
   return result;
};

const sendCookiesTokenInfo = (res, tokenInfo, sendResponse) => {
    const d = new Date(0);
    d.setUTCSeconds(tokenInfo.expires);
    res.cookie(globalConfig.TokenCookie, tokenInfo.token, cookieObject(d, true));
    res.cookie(globalConfig.AntiForgeryTokenCookie, tokenInfo.antiForgeryToken, cookieObject(d, false));
    if (sendResponse===true) {
        res.send(tokenInfo);
    }
}

const clearCookiesTokenInfo = (res) => {
    res.clearCookie(globalConfig.TokenCookie, cookieObject(null, true));
    res.clearCookie(globalConfig.AntiForgeryTokenCookie, cookieObject(null, false));
}

const authenticate = passport.authenticate(["cookie-antiforgery"], { session: false });

const authorize = async (req, res, next) => {
    const url = req.originalUrl;
    const urlParts = url.split("/");
    let authorized = false;

    // logger.info("Passport Authorize");

    if (urlParts.length > 0 && urlParts[0].length === 0) {
        urlParts.shift();
    }
    const urlPartials = [];
    for (let i = urlParts.length; i > 0; i--) {
        let urlPartial = "";
        for (let j = 0; j < i; j++) {
            urlPartial = urlPartial + "/" + urlParts[j].toLowerCase();
        }
        urlPartials.push(urlPartial);
    }

    const query = {
        $and: [{ verb: req.method.toLowerCase() }, { route: { $in: urlPartials } }]
    };
    try {
        const routeAuthorizations = await routeAuthorizationModel
            .find(query)
            .sort([["route", "descending"]]);
        let routeAuthorization = [];
        for (let i = 0; i < urlPartials.length; i++) {
            for (let j = 0; j < routeAuthorizations.length; j++) {
                let currentAuthorization = routeAuthorizations[j].toObject();
                if (urlPartials[i] === currentAuthorization.route) {
                    routeAuthorization.push(currentAuthorization);
                }
            }
            if (routeAuthorization.length > 0) {
                break;
            }
        }
        for (let i = 0; i < routeAuthorization.length; i++) {
            let currentAuthorization = routeAuthorization[i];
            currentAuthorization.project = currentAuthorization.project.toLowerCase();
            currentAuthorization.role = currentAuthorization.role.toLowerCase();
            if (
                (currentAuthorization.project === "*" ||
                    currentAuthorization.project === req.user.project) &&
                (currentAuthorization.role === "*" ||
                    req.user.roles.indexOf(currentAuthorization.role) >= 0) &&
                (currentAuthorization.user === "*" ||
                    currentAuthorization.user === req.user._id.toString())
            ) {
                logger.info("Valid authorization: ", currentAuthorization);
                authorized = true;
                break;
            }
        }
    } catch (ex) {
        logger.info("ERROR IN AUTHORIZATION: " + ex);
        authorized = false;
    }

    // logger.info("PARTIAL: "+urlPartials);
    // logger.info("METHOD: ",req.method);

    if (!authorized) {
        res.sendStatus(401);
    } else {
        next();
    }
};

module.exports = {
    initializeAuthentication,
    getUserFromRequest,
    getToken,
    verifyToken,
    getTokenForUser,
    sendCookiesTokenInfo,
    clearCookiesTokenInfo,
    authenticate,
    authorize
}
