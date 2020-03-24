'use strict';
const mongoose = require("mongoose");
const sessionModel = require("../models/session");
const cryptoController = require("../controllers/cryptoController");

const createSession = async (
  userIdObject,
  antiForgeryToken,
  ipAddress,
  userAgent,
  issuedAt,
  expiresAt
) => {
  const session = {
    ids: cryptoController.generateId("SessionId"),
    userId: userIdObject, 
    antiForgeryToken: antiForgeryToken,
    ipAddress: ipAddress,
    userAgent: userAgent,
    issuedAtDate: new Date(issuedAt * 1000),
    expiresAtDate: new Date(expiresAt * 1000)
  };
  const dbSession = await sessionModel.create(session);
    // dbSession.userId = userIdObject;
    // await sessionModel.updateOne(
    //   { _id: dbSession._id },
    //   { $set: { userId: userIdObject } }
    // );
  return dbSession.toObject();
};

const getValidSessionById = async sessionId => {
  const currentDate = new Date();
  const query = {
    $and: [
      { ids: sessionId },
      { issuedAtDate: { $lte: currentDate } },
      { expiresAtDate: { $gte: currentDate } }
    ]
  };
  let dbSession = await sessionModel.findOne(query);
  return dbSession!==null?dbSession.toObject():null;
};

const invalidateSessionById = async sessionId => {
  const query = { ids: sessionId };
  const updated = { expiresAtDate: new Date(0) };
  await sessionModel.updateOne(query, updated);
};

module.exports = {
    createSession,
    getValidSessionById,
    invalidateSessionById
}
  