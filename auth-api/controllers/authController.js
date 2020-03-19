const passport = require("passport");
const JwtStrategy = require("passport-jwt").Strategy;
const ExtractJwt = require("passport-jwt").ExtractJwt;
const CustomStrategy = require("passport-custom").Strategy;
const globalConfig = require("../config_api.js");
const jsonWebToken = require("jsonwebtoken");
const userController = require("./userController");
const routeAuthorizationModel = require("../models/route_authorization");
const sessionController = require("../controllers/sessionController");
const cryptoController = require("../controllers/cryptoController");
const logger = require('../logger');


const JWT_EXPIRES_AFTER_HOURS = 24 * 7;

const jwtOptions = {
  issuer: "ateventhorizon.com",
  algorithm: "HS384"
};

exports.InitializeAuthentication = () => {

  const cookieExtractor = function(req) {
    // logger.info("COOKIE EXTRACTOR");
    var token = null;
    if (req && req.signedCookies && req.signedCookies["eh_jwt"]) {
      token = req.signedCookies["eh_jwt"];
    }
    return token;
  };

  //
  //Configure jwt strategy
  //
  const jwtOptions = {
    jwtFromRequest: ExtractJwt.fromExtractors([
      cookieExtractor
    ]),
    secretOrKey: globalConfig.JWTSecret,
    issuer: "ateventhorizon.com",
    algorithm: "HS384"
  };

  passport.use("cookie-antiforgery", 
    new CustomStrategy(async (req, done) => {

      let error = null;
      let user = false;
      let session = null;
      try {
        //logger.info("Anti forgery token strategy");
        //Get jwt token from signed cookies and anti forgery token from headers
        jwtToken=null;
        if (req && req.signedCookies && req.signedCookies["eh_jwt"]) {
          jwtToken = req.signedCookies["eh_jwt"];
        }
        aftToken=null;
        if (req && req.headers && req.headers["eh_aft"]) {
          aftToken = req.headers["eh_aft"];
        }   
        
        if (jwtToken===null || aftToken===null) {
          throw "Invalid tokens";
        }
        // Extract token payload
        jwtPayload=jsonWebToken.verify(
          jwtToken,
          globalConfig.JWTSecret,
          jwtOptions
        );      

        //logger.info("JWT PAYLOAD", jwtPayload);
        //Check user in payload

        const sessionId = jwtPayload.sub;
        session = await sessionController.getValidSessionById(sessionId);
        //logger.info("Session:",session);
        if (session !== null) {
          //Check antiforgery token
          if (aftToken!==session.antiForgeryToken) {
            error = `Invalid session ${sessionId}`;
          } else {
            //Get session user
            user = await userController.getUserById(session.userId);
            //logger.info("User: ", user);
            if (user === null) {
              error = "Invalid user";
            } else {
              user.roles = user.roles.map(v => v.toLowerCase());
              user.project = session.project;
              user.sessionId = sessionId;
              user.expires = jwtPayload.exp;
              user.hasToken = true;
              user.hasSession = true;
            }
          }
        } else {
          error = `Invalid session ${sessionId}`;
        }
        //logger.info("Store user: ", user);
      } catch (ex) {
        logger.error(`Error in token: ${ex}`);
        error = "Invalid user";
      }

      if (error !== null) {
        done(null, false, { message: error });
      } else {
        done(null, user);
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

exports.getToken = async (userId, ipAddress, userAgent) => {
  const issuedAt = Math.floor(Date.now() / 1000);
  const expiresAt = issuedAt + 60 * 60 * JWT_EXPIRES_AFTER_HOURS;
  const antiForgeryToken = cryptoController.generateId("AntiForgeryToken");

  const session = await sessionController.createSession(
    userId,
    antiForgeryToken,
    ipAddress,
    userAgent,
    issuedAt,
    expiresAt
  );
  const jwt = await createJwtToken(session.ids, issuedAt, expiresAt);

  return {
    session: session.ids,
    antiForgeryToken: antiForgeryToken,
    token: jwt,
    expires: expiresAt
  };
};

exports.verifyToken = async jwtToken => {
  return await jsonWebToken.verify(
    jwtToken,
    globalConfig.JWTSecret,
    jwtOptions
  );
};

exports.authenticate = passport.authenticate(
  ["cookie-antiforgery"],
  {
    session: false
  }
);

exports.authorize = async (req, res, next) => {
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
