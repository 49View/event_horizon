const express = require("express");
const userController = require("../controllers/userController");
const authController = require("../controllers/authController");
const sessionController = require("../controllers/sessionController");
const logger = require("../logger");
const globalConfig = require("../config_api");
const dataSanitizers = require("../helpers/dataSanitizers");

const router = express.Router();

const cookieObject = (d,httpOnly) => {
  console.log("Cloud host:", globalConfig.CloudHost);
  const cookieDomain = globalConfig.CloudHost === "localhost" ? globalConfig.CloudHost : `.${globalConfig.CloudHost}`;
  const result={
    domain: cookieDomain,
    httpOnly: httpOnly,
    sameSite: "Lax",
    signed: true,
    secure: false,
  };
  if (d!==null) {
    result["expires"]=d;
  }
  return result;
};

router.put(
  "/cleanToken",
  authController.authenticate,
  async (req, res, next) => {
    if (req.user === undefined || req.user === null) {
      res.status(401).send();
    } else {
      if (req.user.hasToken === true) {
        const sessionId = req.user.sessionId;

        await sessionController.invalidateSessionById(sessionId);
        res.clearCookie("eh_aft", cookieObject(null,false));
        res.clearCookie("eh_jwt", cookieObject(null,true));
        res.status(204).send();
      } else {
        res.status(401).send();
      }
    }
  }
);

router.post(
  "/refreshToken",
  authController.authenticate,
  async (req, res, next) => {
    // console.log(req.user);
    if (req.user === undefined || req.user === null) {
      res.status(401).send();
    } else {
      let error = false;
      let tokenInfo = null;
      const ipAddress = req.ip;
      const userAgent = req.headers["user-agent"] || null;

      try {
        if (req.user.hasToken === true) {
          const sessionId = req.user.sessionId;

          await sessionController.invalidateSessionById(sessionId);
          tokenInfo = await authController.getToken(
            req.user._id,
            ipAddress,
            userAgent
          );
          tokenInfo.user = {
            name: req.user.name,
            email: req.user.email,
            guest: req.user.guest
          };
        } else {
          throw new Error("Can't refresh token");
        }
      } catch (ex) {
        console.log("Error refreshing token", ex);
        error = true;
      }
      if (error) {
        res.status(401).send();
      } else {
        const d = new Date(0);
        d.setUTCSeconds(tokenInfo.expires);
        res.cookie("eh_jwt", tokenInfo.token, cookieObject(d, true));
        res.cookie("eh_aft", tokenInfo.antiForgeryToken, cookieObject(d, false));
        res.send(tokenInfo);
      }
    }
  }
);

const getTokenResponse = async (res, req, email, password) => {
  let error = false;
  let errmessage = "";
  let tokenInfo = null;
  const ipAddress = req.ip;
  const userAgent = req.headers["user-agent"] || null;

  try {
    let dbUser = null;

    dbUser = await userController.getUserByEmailPassword(
      email,
      password
    );

    if (dbUser === null) {
      error = true;
      errmessage = "Username, password or interstellar alignment not quite right";
    } else {
      tokenInfo = await authController.getToken(
        dbUser._id,
        ipAddress,
        userAgent
      );
      tokenInfo.user = {
        name: dbUser.name,
        email: dbUser.email,
        guest: dbUser.guest
      };
    }
  } catch (ex) {
    console.log("gettoken failed", ex);
    errmessage = ex;
    error = true;
  }

  if (error === null) {
    res.status(400).send();
  } else if (error === true) {
    res.status(401).send(errmessage);
  } else {
    const d = new Date(0);
    d.setUTCSeconds(tokenInfo.expires);
    res.cookie("eh_jwt", tokenInfo.token, cookieObject(d, true));
    res.cookie("eh_aft", tokenInfo.antiForgeryToken, cookieObject(d, false));
    res.send(tokenInfo);

  }
};

router.post("/getToken", async (req, res, next) => {
  logger.info("/getToken");

  paramsDef = [
    { name: "email", type: "email", required: true},
    { name: "password", type: "string", required: true, min: 8}
  ];
  const [params,error] = dataSanitizers.checkBody(req, paramsDef);
  if (error!==null) {
    res.status(401).send(error);
  } else {
    logger.info(params);
    await getTokenResponse(res, req, params.email, params.password);
  }
});

module.exports = router;
