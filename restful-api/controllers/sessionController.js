const mongoose = require('mongoose');
const sessionModel = require('../models/session');
const ObjectId = mongoose.Types.ObjectId;

exports.createSession = async (userId, project, ipAddress, userAgent, issuedAt, expiresAt) => {
    const session = {
        userId: userId,
        project: project,
        ipAddress: ipAddress,
        userAgent: userAgent,
        issuedAt: issuedAt,
        expiresAt: expiresAt,
        valid: true
    };

    dbSession = await sessionModel.create(session);
    dbSession=dbSession.toObject();

    return dbSession;
}

exports.getValidSessionById = async (sessionId) => {

    const currentDate = Math.floor(new Date() / 1000);
    const query = { "$and": [ 
        {_id: ObjectId(sessionId)},
        {valid: true},
        {issuedAt: {$lte: currentDate }},
        {expiresAt: {$gte: currentDate}}
    ]};
    // console.log(query);
    // console.log(query["$and"]);
    // console.log(query["$and"]);
    dbSession = await sessionModel.findOne(query);
    if (dbSession!==null) {
        dbSession=dbSession.toObject();
    }
    return dbSession;
}

exports.invalidateSessionById = async (sessionId) => {

    const query = { _id: ObjectId(sessionId)};
    const updated = { valid: false};
    await sessionModel.updateOne(query, updated);
}

