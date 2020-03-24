'use strict';
const crypto = require("crypto");
const mongoose = require("mongoose");
const sha256 = require("sha256");
const ObjectId = mongoose.Types.ObjectId;

const generateId = (prefix) => {
  return sha256(prefix + crypto.randomBytes(8).toString("base64") + new ObjectId());
}

module.exports = {
    generateId
}
