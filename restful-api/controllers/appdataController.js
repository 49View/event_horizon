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

exports.addAppWithName = async (project, appName) => {
  try {
    const rs = {
      shadowOverBurnCofficient: 2,
      indoorSceneCoeff: 1,
      shadowZFightCofficient: 0.002
    };

    const content = {
      mKey: appName,
      renderSettings: rs,
      project: project,
      group: "app",
      geoms: [],
      colors: [],
      materials: [],
      profiles: [],
      images: [],
      fonts: [],
      scripts: []
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

exports.getApp = async key => {
  try {
    const ret = await appdataModel.findOne({ mKey: key });
    return ret.toObject();
  } catch (error) {
    console.log("Error adding app to project ", error);
  }
};

exports.updateApp = async content => {
  try {
    const ret = await appdataModel.findOneAndUpdate(
      { mKey: content.mKey },
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
