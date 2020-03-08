const express = require("express");
const projectController = require("../controllers/projectController");
const appdataController = require("../controllers/appdataController");
const userController = require("../controllers/userController");
const mailController = require("../controllers/mailController");
const Base64 = require("js-base64").Base64;

const router = express.Router();

router.get("/myProject", async (req, res, next) => {
  let dbUsers = null;
  try {
    dbUsers = await userController.getUsersByProject(req.user.project);
  } catch (ex) {
    console.log("Error getting users for project " + req.user.project);
    dbUsers = null;
  }
  dbUsers === null ? res.sendStatus(400) : res.json(dbUsers);
});

const getUser = async req => {
  const result = {
    expires: req.user.expires,
    user: {name: req.user.name, email: req.user.email, guest: req.user.guest},
    project: req.user.project !== "" ? req.user.project : null,
    projects: [],
    invitations: []
  };
  if (req.user.hasSession === true) {
    result.session = req.user.sessionId;
  }
  result.projects = await userController.getUserProjects(req.user._id);
  result.invitations = await userController.getUserInvites(req.user.email);

  return result;
}

router.get("/", async (req, res, next) => {
  // console.log("USER GET /");
  res.send(await getUser(req));
});

router.put("/addRolesFor", async (req, res, next) => {
  console.log('User addRolesFor "' + req.body.project + '": ', req.body);

  const project = req.body.project;
  const email = req.body.email;
  const roles = req.body.roles;

  try {
    await userController.addRolesForProject(project, email, roles);
    res.send(await getUser(req));
  } catch (ex) {
    res.status(400).send("Error adding roles for project " + ex.message)
  }
});

router.post("/createProject", async (req, res, next) => {
  console.log("Create project ", req.body.project);

  const project = req.body.project;
  const email = req.user.email;
  const roles = ["admin", "user"];

  try {
    if ( project === "" ) {
      throw "ehmmm, where's the project???";
    }
    // add default app for this project
    if ((await userController.checkProjectAlreadyExists(project)) === true) {
      throw "Project exists already";
    }
    await userController.addRolesForProject(project, email, roles);
    res.send(await getUser(req));
  } catch (ex) {
    res.status(400).send(ex);
  }
});

router.put("/acceptInvitation", async (req, res,) => {
  console.log('User addRolesFor "' + req.body.project + '": ', req.body);

  const project = req.body.project;
  const email = req.body.email;
  const roles = ["user"];

  try {
    await userController.addRolesForProject(project, email, roles);
    await userController.removeInvitation({
      persontoadd: email,
      project: project
    });
    res.send(await getUser(req));
  } catch (ex) {
    res.status(400).send(ex.message)
  }

});

router.delete("/invitetoproject", async (req, res, next) => {
  try {
    await userController.removeInvitation(req.body);
    res.send(await getUser(req));
  } catch (ex) {
    res.status(400).send(ex.message)
  }
});

router.put("/invitetoproject", async (req, res, next) => {
  try {
    const projectUsers = await userController.getUsersByProject(
      req.body.project
    );
    let userEmail = await userController.getUserByName(req.body.persontoadd);
    if (userEmail === null) {
      userEmail = await userController.getUserByEmail(req.body.persontoadd);
      if (userEmail === null) {
        console.log("User you are trying to invite doesn't exist");
        res.status(400).send(req.body.persontoadd + " doesn't exist in here");
        return;
      }
    }
    // Check if the user is already part of the project
    const isAlreadyInThere = projectUsers.reduce((present, e) => {
      return present + (e.email === userEmail.email);
    }, 0);
    if (isAlreadyInThere > 0) {
      console.log("User already in project ", isAlreadyInThere);
      res.status(400).send(req.body.persontoadd + " already in project");
      return;
    }

    // Check if the user has already an invitation pending to this project
    if (
      await userController.getUserInvitationToProject(
        userEmail.email,
        req.body.project
      )
    ) {
      console.log("User already been invited");
      res.status(400).send(req.body.persontoadd + " has already been invited");
      return;
    }

    await userController.addInvitation(userEmail.email, req.body.project);

    const plainText =
      "Hi " +
      req.body.persontoadd +
      "\n\n" +
      req.body.adminuser +
      " has invited you to join " +
      req.body.project +
      " Login to eventhorizon if you want to check it out, you'll find it in your pending invitations section.";
    const html =
      "Hi <strong>" +
      req.body.persontoadd +
      "</strong><br><br><strong>" +
      req.body.adminuser +
      " </strong>has invited you to join <strong>" +
      req.body.project +
      "</strong><br><br>Login to eventhorizon if you want to check it out, you'll find it in your pending invitations section.<br><br>" +
      `<table width="100%" cellspacing="0" cellpadding="0">
      <tr>
          <td>
              <table cellspacing="0" cellpadding="0">
                  <tr>
                      <td style="border-radius: 2px;" bgcolor="#28a745">
                          <a href="https://www.ateventhorizon.com/#/dashboarduser" target="_blank" style="padding: 8px 12px; border: 3px solid #28a745;border-radius: 2px;font-family: Helvetica, Arial, sans-serif;font-size: 14px; color: #ffffff;text-decoration: none;font-weight:bold;display: inline-block;">
                              Check it out
                          </a>
                      </td>
                  </tr>
              </table>
          </td>
      </tr>
    </table>`;

    await mailController.sendMail(
      userEmail,
      "EventHorizon@notifications.invite",
      req.body.adminuser + " has invited you to join " + req.body.project,
      plainText,
      html
    );

    res.status(200).send({
      code: 200,
      msg: req.body.persontoadd + " has been invited"
    });
  } catch (ex) {
    console.log("Error sending invite for project ", ex);
    res.sendStatus(400);
  }
});

router.post("/addProjectApp/:project/:appname", async (req, res, next) => {
  try {
    await projectController.addApp({
      name: req.params.appname,
      project: req.params.project
    });
    res.sendStatus(204);
  } catch (ex) {
    console.log("Error adding app for project ", ex);
    res.sendStatus(400);
  }
});

router.put("/removeRolesForProject", async (req, res, next) => {
  console.log("User removeRolesForProject: ", req.body);

  const project = req.body.project;
  const email = req.body.email;
  const roles = req.body.roles;
  let error = false;

  try {
    await userController.removeRolesForProject(project, email, roles);
  } catch (ex) {
    console.log("Error removing roles for project", ex);
    error = true;
  }

  error === null ? res.sendStatus(400) : res.sendStatus(204);
});

router.delete("/project/:project", async (req, res, next) => {
  try {
    const project = req.params.project;
    await userController.removeAllRolesForProject(project);
    await appdataController.deleteApp(project);
    res.sendStatus(204);
  } catch (ex) {
    console.log("Error deleting project ", ex);
    res.sendStatus(400);
  }
});

module.exports = router;
