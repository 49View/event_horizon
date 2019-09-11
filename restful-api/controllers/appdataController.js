const mongoose = require("mongoose");
const appdataModel = require("../models/appdata");
const metadataAssistant = require("../assistants/metadataAssistant");

const addAppInternal = async content => {
  try {
    const newEntityDB = new appdataModel(content);
    await newEntityDB.save();
    return newEntityDB.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

const replaceAppInternal = async content => {
  try {
    return await appdataModel.findOneAndReplace(
      { project: content.project },
      content
    );
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.addApp = async project => {
  try {
    const rs = {
      shadowOverBurnCofficient: 2,
      indoorSceneCoeff: 1,
      shadowZFightCofficient: 0.002
    };

    const content = {
      renderSettings: rs,
      project: project,
      group: "app",
      entities: []
    };
    content.metadata = metadataAssistant.createMetadata(content);

    return await addAppInternal(content);
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.addApp = async content => {
  try {
    return await addAppInternal(content);
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.replaceApp = async content => {
  try {
    return await replaceAppInternal(content);
  } catch (error) {
    console.log("Error replacing app to project ", error);
  }
};

exports.getApp = async project => {
  try {
    const ret = await appdataModel.findOne({ project: project });
    return ret.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.updateApp = async content => {
  try {
    const ret = await appdataModel.findOneAndUpdate(
      { project: content.project },
      content,
      {
        new: true
      }
    );
    return ret.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.deleteApp = async project => {
  const query = { project: project };
  await appdataModel.deleteOne(query);
};

exports.listAppsForProject = async project => {
  try {
    return await appdataModel.find({ project: project });
  } catch (error) {
    console.log("Error listing apps beloning to project ", error);
  }
};
