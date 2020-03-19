'use strict';
const mongoose = require("mongoose");
const crypto = require("crypto");
const userModel = require("../models/user");
const asyncModelOperations = require("../helpers/asyncModelOperations");
const ObjectId = mongoose.Types.ObjectId;
const logger = require('../logger');


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

const getUserByNameInternal = async name => {
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

exports.isEmailinUse = async email => {
  return await getUserByEmailInternal(email) !== null;
}

exports.isNameinUse = async name => {
  return await getUserByNameInternal(name) !== null;
}

exports.getUserById = async id => {
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

exports.getUserByEmail = async email => {
  return await getUserByEmailInternal(email);
};

exports.getUserByName = async name => {
  return await getUserByNameInternal(name);
};

exports.createUser = async (name, email, password) => {
  let dbUser = null;

  // Check is email has already been used
  if ( await getUserByEmailInternal(email) !== null ) {
    return null;
  }
  // Check name has already been used
  if ( await getUserByNameInternal(name) !== null ) {
    return null;
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

exports.getUserByEmailPassword = async (email, password) => {
  let dbUser = await getUserByEmailInternal(email);
  if (dbUser !== null) {
    logger.info("User exists ");
    const cipherPasswordParts = dbUser.cipherPassword.split("$");
    const hash = createCipherPassword(cipherPasswordParts[0], password);
    delete dbUser.cipherPassword;
    if (hash !== cipherPasswordParts[1]) {
      dbUser = null;
      logger.info("User specify invalid password");
    }
  }
  return dbUser;
};

function createCipherPassword(salt,password) {
  return crypto
  .createHmac("sha512", salt)
  .update(password)
  .digest("base64");

}
