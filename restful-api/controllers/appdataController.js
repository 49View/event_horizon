const mongoose = require("mongoose");
const appdataModel = require("../models/appdata");

exports.addApp = async content => {
  try {
    const newEntityDB = new appdataModel(content);
    await newEntityDB.save();
    return newEntityDB.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.getApp = async key => {
  try {
    const ret = await appdataModel.findOne({ mKey: key });
    return ret.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};
