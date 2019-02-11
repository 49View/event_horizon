const mongoose = require('mongoose');
const sessionModel = require('../models/session');
//const ObjectId = mongoose.Types.ObjectId;

exports.createSession = async (userId, project, ipAddress, userAgent, issuedAt, expiresAt) => {


    const expiresAtDate = new Date(expiresAt*1000);

    const session = {
        userId: userId,
        project: project,
        ipAddress: ipAddress,
        userAgent: userAgent,
        issuedAt: issuedAt,
        expiresAt: expiresAt,
        expiresAtDate: expiresAtDate
    };

    dbSession = await sessionModel.create(session);
    dbSession=dbSession.toObject();

    return dbSession;
}

exports.getValidSessionById = async (sessionId) => {

    const currentDate = Math.floor(new Date() / 1000);
    const query = { "$and": [ 
        // {_id: ObjectId(sessionId)},
        {_id: sessionId},
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

    const expiresAtDate = new Date((new Date())-10000);
    // const query = { _id: ObjectId(sessionId)};
    const query = { _id: sessionId};
    const updated = { expiresAtDate: expiresAtDate};
    await sessionModel.updateOne(query, updated);
}

