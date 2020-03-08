const mongoose = require("mongoose");
const crypto = require("crypto");
const userModel = require("../models/user");
const userRoleModel = require("../models/user_role");
const userStateModel = require("../models/user_state");
const userProjectInvitationModel = require("../models/user_project_invitation");
const asyncModelOperations = require("../assistants/asyncModelOperations");

const ObjectId = mongoose.Types.ObjectId;

const getUsersByProject = async project => {
  const query = [
    {
      $match: {
        project: { $regex: project + "$", $options: "i" }
      }
    },
    {
      $lookup: {
        from: "users",
        localField: "userId",
        foreignField: "_id",
        as: "user"
      }
    },
    {
      $unwind: {
        path: "$user"
      }
    },
    {
      $project: {
        _id: "$user._id",
        name: "$user.name",
        email: "$user.email",
        guest: "$user.guest",
        project: "$project",
        roles: "$roles"
      }
    }
  ];

  return await asyncModelOperations.aggregate(userRoleModel, query);
};

const getUserByEmailInternal = async email => {
  let dbUser = null;
  const query = { email: { $regex: email + "$", $options: "i" } };

  dbUser = await userModel.findOne(query);
  if (dbUser !== null) {
    dbUser = dbUser.toObject();
    return dbUser;
  } else {
    return null;
  }
};

exports.isEmailinUse = async email => {
  return await getUserByEmailInternal(email) !== null;
}

exports.getUserByEmail = async email => {
  return await getUserByEmailInternal(email);
};

exports.getUserByName = async name => {
  let dbUser = null;
  const query = { name: { $regex: name + "$", $options: "i" } };

  dbUser = await userModel.findOne(query);
  if (dbUser !== null) {
    dbUser = dbUser.toObject();
    return dbUser;
  } else {
    return null;
  }
};

exports.checkProjectAlreadyExists = async project => {
  let dbUser = null;
  const query = { project: { $regex: project + "$", $options: "i" } };
  dbUser = await userRoleModel.find(query);
  return dbUser !== null && dbUser.length > 0 ? true : false;
};

exports.addInvitation = async (persontoadd, project) => {
  let dbUser = null;
  const query = { persontoadd: persontoadd, project: project };

  dbUser = await userProjectInvitationModel.create(query);
  dbUser = dbUser.toObject();

  return dbUser;
};

exports.removeInvitation = async entry => {
  await userProjectInvitationModel.remove(entry);
};

exports.getUserInvitationToProject = async (persontocheck, project) => {
  const query = { persontoadd: persontocheck, project: project };

  const dbUser = await userProjectInvitationModel.findOne(query);
  return dbUser !== null;
};

exports.getUserInvites = async userEmail => {
  const query = { persontoadd: userEmail };

  const invites = await userProjectInvitationModel.find(query);
  return invites !== null ? invites : [];
};

const getUserWithRolesByEmailProject = async (email, project) => {
  let dbUser = null;
  const query = [];

  query.push({
    $match: { email: { $regex: email + "$", $options: "i" }, guest: false }
  });
  query.push({
    $lookup: {
      from: "users_roles",
      localField: "_id",
      foreignField: "userId",
      as: "roles"
    }
  });
  query.push({ $unwind: { path: "$roles" } });
  if (email != "daemon") {
    query.push({
      $match: { "roles.project": { $regex: project + "$", $options: "i" } }
    });
  }
  query.push({
    $group: {
      _id: "$_id",
      name: { $first: "$name" },
      guest: { $first: "$guest" },
      email: { $first: "$email" },
      cipherPassword: { $first: "$cipherPassword" },
      active: { $first: "$active" },
      roles: { $first: "$roles.roles" }
    }
  });

  //console.log(query);
  dbUser = await asyncModelOperations.aggregate(userModel, query);
  if (dbUser.length > 0) {
    dbUser = dbUser[0];
  } else {
    dbUser = null;
  }

  return dbUser;
};

const getUserWithRolesByGuestProject = async project => {
  let dbUser = null;
  const query = [];
  query.push({ $match: { guest: true } });
  query.push({
    $group: {
      _id: "$_id",
      name: { $first: "$name" },
      guest: { $first: "$guest" },
      email: { $first: "$email" },
      cipherPassword: { $first: "$cipherPassword" },
      active: { $first: "$active" },
      roles: { $first: "$roles.roles" }
    }
  });

  dbUser = await asyncModelOperations.aggregate(userModel, query);
  if (dbUser.length > 0) {
    dbUser = dbUser[0];
  } else {
    dbUser = null;
  }

  return dbUser;
};

const getUserWithRolesByIdProject = async (id, project) => {
  let dbUser = null;
  //console.log(id,project);
  const query = [];
  query.push({ $match: { _id: ObjectId(id) } });
  query.push({
    $lookup: {
      from: "users_roles",
      localField: "_id",
      foreignField: "userId",
      as: "roles"
    }
  });
  query.push({ $unwind: { path: "$roles" } });
  query.push({
    $match: { "roles.project": { $regex: project + "$", $options: "i" } }
  });
  query.push({
    $group: {
      _id: "$_id",
      name: { $first: "$name" },
      guest: { $first: "$guest" },
      email: { $first: "$email" },
      cipherPassword: { $first: "$cipherPassword" },
      active: { $first: "$active" },
      roles: { $first: "$roles.roles" }
    }
  });

  dbUser = await asyncModelOperations.aggregate(userModel, query);
  if (dbUser.length > 0) {
    dbUser = dbUser[0];
  } else {
    dbUser = null;
  }

  return dbUser;
};

const getUserProjects = async userId => {
  return await getProjectsByUser(userId);
};

const getProjectsByUser = async id => {
  const query = [];
  query.push({ $match: { userId: ObjectId(id) } });
  query.push({ $match: { project: { $ne: "" } } });

  try {
    return await asyncModelOperations.aggregate(userRoleModel, query);
  } catch (error) {
    return null;
  }
};

exports.createUser = async (name, email, password) => {
  let dbUser = null;

  // Check is email has already been used
  if ( await getUserByEmailInternal(email) !== null ) {
    return null;
  }

  const salt = crypto.randomBytes(16).toString("base64");
  const hash = crypto
    .createHmac("sha512", salt)
    .update(password)
    .digest("base64");

  const cipherPassword = salt + "$" + hash;

  const user = {
    name: name,
    email: email,
    emailConfirmed: false,
    cipherPassword: cipherPassword,
    active: true,
    guest: false
  };

  dbUser = await userModel.create(user);
  dbUser = dbUser.toObject();

  // Add default roles
  await addRolesForProject("", dbUser.email, ["user"]);

  return dbUser;
};

exports.getUserByEmailPassword = async (email, password) => {
  let dbUser = await getUserByEmailInternal(email);
  if (dbUser !== null) {
    const cipherPasswordParts = dbUser.cipherPassword.split("$");
    hash = crypto
      .createHmac("sha512", cipherPasswordParts[0])
      .update(password)
      .digest("base64");
    delete dbUser.cipherPassword;
    if (hash !== cipherPasswordParts[1]) {
      dbUser = null;
    }
  }
  return dbUser;
};

exports.getUserByGuestProject = async project => {
  let dbUser = await getUserWithRolesByGuestProject(project);
  if (dbUser !== null) {
    delete dbUser.cipherPassword;
  }
  return dbUser;
};

exports.getUserByIdProject = async (id, project) => {
  let dbUser = await getUserWithRolesByIdProject(id, project);
  if (dbUser !== null) {
    delete dbUser.cipherPassword;
  }
  return dbUser;
};

const addRolesForProject = async (project, email, roles) => {
  const dbUser = await getUserByEmailInternal(email);
  const query = { $and: [{ userId: dbUser._id }, { project: project }] };
  const update = { $addToSet: { roles: { $each: roles } } };
  const options = { upsert: true };
  await userRoleModel.updateOne(query, update, options);
};

exports.removeRolesForProject = async (project, email, roles) => {
  const dbUser = await getUserByEmailInternal(email);
  const query = { $and: [{ userId: dbUser._id }, { project: project }] };
  const update = { $pull: { roles: { $in: roles } } };
  const options = {};
  await userRoleModel.updateOne(query, update, options);
};

exports.removeAllRolesForProject = async project => {
  const query = { project: project };
  await userRoleModel.deleteOne(query);
};

exports.getUsersByProject = async project => {
  return await getUsersByProject(project);
};

exports.getProjectsByUser = async userId => {
  return await getProjectsByUser(userId);
};

exports.getUserProjects = async userId => {
  return await getUserProjects(userId);
};

exports.addRolesForProject = async (project, email, roles) => {
  return await addRolesForProject(project, email, roles);
};

exports.setCurrentPortalEntity = async (user, entity) => {
  try {
    await userStateModel.findOneAndUpdate(
      { user_id: user._id },
      { user_id: user._id, entity: entity, date: new Date() },
      { upsert: true }
    );
  } catch (error) {
    console.log("ERROR setCurrentPortalEntity: ", error);
  }
};

exports.getCurrentPortalEntity = async (user, entity) => {
  try {
    return await userStateModel.findOne({ user_id: user._id });
  } catch (error) {
    console.log("ERROR getCurrentPortalEntity: ", error);
    return null;
  }
};
