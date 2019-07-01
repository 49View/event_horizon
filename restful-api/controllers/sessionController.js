const sha256 = require("sha256");
const uniqid = require("uniqid");
const sessionModel = require("../models/session");

exports.createSession = async (
  userId,
  project,
  ipAddress,
  userAgent,
  issuedAt,
  expiresAt
) => {
  const issuedAtDate = new Date(issuedAt * 1000);
  const expiresAtDate = new Date(expiresAt * 1000);
  const id = sha256(
    uniqid("A") + "-" + uniqid("B") + "-" + uniqid("C") + "-" + uniqid("D")
  );

  const session = {
    ids: id,
    userId: userId,
    project: project,
    ipAddress: ipAddress,
    userAgent: userAgent,
    // issuedAt: issuedAt,
    // expiresAt: expiresAt,
    issuedAtDate: issuedAtDate,
    expiresAtDate: expiresAtDate
  };

  try {
    console.log("[Session]", session);
    await sessionModel.create(session);
    return session; //dbSession.toObject();
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
  dbSession = await sessionModel.findOne(query);
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
