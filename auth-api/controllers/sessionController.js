'use strict';
const mongoose = require("mongoose");
const sessionModel = require("../models/session");
const ObjectId = mongoose.Types.ObjectId;
const cryptoController = require("../controllers/cryptoController");

exports.createSession = async (
  userIdObject,
  antiForgeryToken,
  ipAddress,
  userAgent,
  issuedAt,
  expiresAt
) => {
  const issuedAtDate = new Date(issuedAt * 1000);
  const expiresAtDate = new Date(expiresAt * 1000);
  // const id = sha256(
  //   uniqid("A") + "-" + uniqid("B") + "-" + uniqid("C") + "-" + uniqid("D")
  // );
  const id = cryptoController.generateId("SessionId");

  const session = {
    ids: id,
    userId: userIdObject, 
    antiForgeryToken: antiForgeryToken,
    ipAddress: ipAddress,
    userAgent: userAgent,
    issuedAtDate: issuedAtDate,
    expiresAtDate: expiresAtDate
  };

  try {
    const dbSession = await sessionModel.create(session);
    dbSession.userId = userIdObject;
    await sessionModel.updateOne(
      { _id: dbSession._id },
      { $set: { userId: userIdObject } }
    );
    return dbSession.toObject();
  } catch (error) {
    console.log("[Catch: (EX) Session Error]", error);
    return null;
  }
  // return session;
};

exports.getValidSessionById = async sessionId => {
  const currentDate = new Date();
  const currentEpoch = Math.floor(currentDate / 1000);
  const query = {
    $and: [
      // {_id: ObjectId(sessionId)},
      { ids: sessionId },
      // {issuedAt: {$lte: currentEpoch }},
      // {expiresAt: {$gte: currentEpoch}},
      { issuedAtDate: { $lte: currentDate } },
      { expiresAtDate: { $gte: currentDate } }
    ]
  };
  // console.log(query);
  // console.log(query["$and"]);
  // console.log(query["$and"]);
  let dbSession = await sessionModel.findOne(query);
  if (dbSession !== null) {
    dbSession = dbSession.toObject();
  }
  // console.log("CURRENT SESSION:",dbSession);
  return dbSession;
};

exports.invalidateSessionById = async sessionId => {
  const expiresAtDate = new Date(new Date() - 10000);
  // const query = { _id: ObjectId(sessionId)};
  const query = { ids: sessionId };
  const updated = { expiresAtDate: expiresAtDate };
  await sessionModel.updateOne(query, updated);
};
