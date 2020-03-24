'use strict';
const crypto = require("crypto");
const userModel = require("../models/user");
const logger = require('../logger');


const getUserById = async id => {
  let dbUser = null;
  const query = { _id: id };

  dbUser = await userModel.findOne(query);
  if (dbUser !== null) {
    dbUser = dbUser.toObject();
    return dbUser;
  } else {
    return null;
  }
}

const getUserByEmail = async email => {
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

const getUserByName = async name => {
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

const isEmailinUse = async email => {
  return await getUserByEmail(email) !== null;
}

const isNameinUse = async name => {
  return await getUserByName(name) !== null;
}

const createUser = async (name, email, password) => {
  let dbUser = null;

  // Check is email has already been used
  if (await isEmailinUse(email)) {
    throw "email already been used";
  }
  // Check name has already been used
  if (await isNameinUse(name)) {
    throw "username already been used";
  }

  const salt = crypto.randomBytes(16).toString("base64");
  const hash = createCipherPassword(salt, password);

  const cipherPassword = salt + "$" + hash;

  const user = {
    name: name,
    email: email,
    emailConfirmed: false,
    cipherPassword: cipherPassword,
    active: true,
    guest: false,
    roles: ["user"]
  };

  dbUser = await userModel.create(user);
  dbUser = dbUser.toObject();
  
  return dbUser;
};

const getUserByEmailPassword = async (email, password) => {
  let dbUser = await getUserByEmail(email);
  if (dbUser === null) {
    throw "Invalid user";
  }
  const cipherPasswordParts = dbUser.cipherPassword.split("$");
  const hash = createCipherPassword(cipherPasswordParts[0], password);
  delete dbUser.cipherPassword;
  if (hash !== cipherPasswordParts[1]) {
    throw "Invalid password";
  }
  return dbUser;
};

const createCipherPassword = (salt,password) => {
  return crypto
  .createHmac("sha512", salt)
  .update(password)
  .digest("base64");

}

module.exports = {
  getUserById,
  getUserByEmail,
  getUserByName,
  isEmailinUse,
  isNameinUse,
  createUser,
  getUserByEmailPassword
}
