const express = require("express");
const userController = require("../controllers/userController");
const authController = require("../controllers/authController");
const sessionController = require("../controllers/sessionController");
const socketController = require("../controllers/socketController");
const logger = require("../logger");

const router = express.Router();

router.get(
  "/cleanToken",
  authController.authenticate,
  async (req, res, next) => {
    if (req.user === undefined || req.user === null) {
      res.status(401).send();
    } else {
      if (req.user.hasToken === true) {
        const sessionId = req.user.sessionId;

        await sessionController.invalidateSessionById(sessionId);
        socketController.closeClientsWithSessionId(sessionId);
        res
          .clearCookie("eh_jwt", {
            httpOnly: true,
            sameSite: "None",
            signed: true,
            secure: false
          })
          .status(200)
          .send();
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
            req.user.project,
            ipAddress,
            userAgent
          );
          await socketController.replaceClientsSession(
            sessionId,
            tokenInfo.session
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

        res
          .cookie("eh_jwt", tokenInfo.token, {
            httpOnly: true,
            sameSite: "None",
            signed: true,
            secure: true,
            expires: d
          })
          .send(tokenInfo);
      }
    }
  }
);

router.post(
  "/refreshToken/:project",
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
            req.params.project,
            ipAddress,
            userAgent
          );
          await socketController.replaceClientsSession(
            sessionId,
            tokenInfo.session
          );
          tokenInfo.user = {
            name: req.user.name,
            email: req.user.email,
            guest: req.user.guest
          };
          tokenInfo.project = req.params.project;
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

        res
          .cookie("eh_jwt", tokenInfo.token, {
            httpOnly: true,
            sameSite: "None",
            signed: true,
            secure: true,
            expires: d
          })
          .send(tokenInfo);
      }
    }
  }
);

const getTokenResponse = async (res, req, project, email, password) => {
  let error = false;
  let errmessage = "";
  let tokenInfo = null;
  const ipAddress = req.ip;
  const userAgent = req.headers["user-agent"] || null;

  try {
    let dbUser = null;

    if (email == "guest") {
      dbUser = await userController.getUserByGuestProject(
        email,
        project,
        password
      );
    } else {
      dbUser = await userController.getUserByEmailPasswordProject(
        email,
        project,
        password
      );
    }

    if (dbUser === null) {
      error = true;
    } else {
      tokenInfo = await authController.getToken(
        dbUser._id,
        project,
        ipAddress,
        userAgent
      );
      tokenInfo.user = {
        name: dbUser.name,
        email: dbUser.email,
        guest: dbUser.guest
      };
      tokenInfo.project = project !== "" ? project : ""; //await userController.setDefaultUserProject(dbUser._id);
    }
  } catch (ex) {
    console.log("gettoken failed", ex);
    errmessage = ex;
    error = true;
  }

  if (error === null) {
    res.status(400).send();
  } else if (error) {
    res.status(401).send(errmessage);
  } else {
    const d = new Date(0);
    d.setUTCSeconds(tokenInfo.expires);

    res
      .cookie("eh_jwt", tokenInfo.token, {
        httpOnly: true,
        sameSite: "None",
        signed: true,
        secure: true,
        expires: d
      })
      .send(tokenInfo);
  }
};

router.post("/getToken", async (req, res, next) => {
  logger.info( "/getToken" );

  let project = req.body.project;
  const email = req.body.email;
  const password = req.body.password;

  logger.info( "Project: " + project + " email: " + email );
  await getTokenResponse(res, req, project, email, password);
});

router.post("/createuser", async (req, res, next) => {
  console.log("User create: ", req.body);

  const name = req.body.name;
  const email = req.body.email;
  const password = req.body.password;
  let dbUser = null;

  try {
    dbUser = await userController.createUser(name, email, password);
  } catch (ex) {
    console.log("Error creating user", ex);
    dbUser = null;
  }

  if (dbUser === null) {
    res.sendStatus(400);
  } else {
    await getTokenResponse(res, req, "", email, password);
  }
});

module.exports = router;
