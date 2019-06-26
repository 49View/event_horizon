const express = require("express");
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
  // res.send("dado");
  //console.log(req.user);
  const result = {
    expires: req.user.expires,
    user: { name: req.user.name, email: req.user.email, guest: req.user.guest },
    project: req.user.project
  };
  if (req.user.hasSession === true) {
    result.session = req.user.sessionId;
  }
  if (result.project === "") {
    result.project = await userController.setDefaultUserProject(req.user._id);
  }

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

module.exports = router;
