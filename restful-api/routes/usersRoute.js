const express = require("express");
const projectController = require("../controllers/projectController");
const appdataController = require("../controllers/appdataController");
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
    // add default app for this project
    await appdataController.addAppWithName(project, project + "App");
    await userController.addRolesForProject(project, email, roles);
  } catch (ex) {
    console.log("Error creating project", ex);
    error = true;
  }

  error === null ? res.sendStatus(400) : res.sendStatus(204);
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
