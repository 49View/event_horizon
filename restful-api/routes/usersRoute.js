const express = require("express");
const projectController = require("../controllers/projectController");
const userController = require("../controllers/userController");

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

router.get("/", async (req, res, next) => {
  // console.log("USER GET /");
  const result = {
    expires: req.user.expires,
    user: { name: req.user.name, email: req.user.email, guest: req.user.guest },
    project: req.user.project,
    projects: []
  };
  if (req.user.hasSession === true) {
    result.session = req.user.sessionId;
  }
  result.projects = await userController.getUserProjects(req.user._id);
  // if (result.project === "" && result.projects.length == 1) {
  //   result.project = result.projects[0];
  // }

  res.send(result);
});

router.put("/addRolesFor/:project", async (req, res, next) => {
  console.log('User addRolesFor "' + req.params.project + '": ', req.body);

  const project = req.params.project;
  const email = req.body.email;
  const roles = req.body.roles;
  let error = false;

  try {
    await userController.addRolesForProject(project, email, roles);
  } catch (ex) {
    console.log("Error adding roles for project", ex);
    error = true;
  }

  error === null ? res.sendStatus(400) : res.sendStatus(204);
});

router.post("/createProject/:project", async (req, res, next) => {
  console.log("Create project ", req.params.project);

  const project = req.params.project;
  const email = req.user.email;
  const roles = ["admin", "user"];
  let error = false;

  try {
    await userController.addRolesForProject(project, email, roles);
    // add default app for this project
    const content = JSON.stringify({});
    await projectController.addApp(project, project + "App", content);
  } catch (ex) {
    console.log("Error creating project", ex);
    error = true;
  }

  error === null ? res.sendStatus(400) : res.sendStatus(204);
});

router.post("/addProjectApp/:project/:appname", async (req, res, next) => {
  try {
    const content = JSON.stringify({});
    await projectController.addApp(
      req.params.project,
      req.params.appname,
      content
    );
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

// router.get("/portaltoload", async (req, res, next) => {
//   try {
//     console.log(req.user);
//     const ret = await userController.getCurrentPortalEntity(req.user);
//     res.status(200).send(ret.toObject().entity);
//   } catch (ex) {
//     console.log("ERROR [GET] portaltoload: ", ex);
//     res.sendStatus(400);
//   }
// });

// router.post("/portaltoload", async (req, res, next) => {
//   try {
//     await userController.setCurrentPortalEntity(req.user, req.body);
//     res.sendStatus(204);
//   } catch (ex) {
//     console.log("ERROR [POST] portaltoload: ", ex);
//     res.sendStatus(400);
//   }
// });

module.exports = router;
