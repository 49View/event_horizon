const mongoose = require('mongoose');
const crypto = require('crypto');
const userModel = require('../models/user');
const userRoleModel = require('../models/user_role');
const asyncModelOperations = require('../assistants/asyncModelOperations');

const ObjectId = mongoose.Types.ObjectId;

const getUserByEmail = async (email) => {
    
    let dbUser = null;
    const query = {"email": email};

    dbUser=await userModel.findOne(query);
    dbUser=dbUser.toObject();

    return dbUser;
}

const getUserWithRolesByEmailProject = async (email,project) => {
    
    let dbUser = null;
    const query = [];
    query.push({ $match: {"email": email}});
    query.push({ $lookup: {"from": "users_roles", "localField": "_id", "foreignField": "userId", "as": 'roles'}});
    query.push({ $unwind: {"path": "$roles"}});
    query.push({ $match: {"roles.project": project}});  
    query.push({ $group: {"_id": "$_id", "name": { "$first": "$name" }, "email": { "$first": "$email" }, "cipherPassword": { "$first": "$cipherPassword" }, "active": { "$first": "$active" }, "roles": { "$first": "$roles.roles" }}});

    dbUser=await asyncModelOperations.aggregate(userModel,query);
    if (dbUser.length>0) {
        dbUser=dbUser[0];
    } else {
        dbUser=null;
    }

    return dbUser;
}

const getUserWithRolesByIdProject = async (id,project) => {
    
    let dbUser = null;
    const query = [];
    query.push({ $match: {"_id": ObjectId(id)}});
    query.push({ $lookup: {"from": "users_roles", "localField": "_id", "foreignField": "userId", "as": 'roles'}});
    query.push({ $unwind: {"path": "$roles"}});
    query.push({ $match: {"roles.project": project}});  
    query.push({ $group: {"_id": "$_id", "name": { "$first": "$name" }, "email": { "$first": "$email" }, "cipherPassword": { "$first": "$cipherPassword" }, "active": { "$first": "$active" }, "roles": { "$first": "$roles.roles" }}});

    dbUser=await asyncModelOperations.aggregate(userModel,query);
    if (dbUser.length>0) {
        dbUser=dbUser[0];
    } else {
        dbUser=null;
    }

    return dbUser;
}

exports.createUser = async (name, email, password) => {    
    let dbUser = null;
    const salt = crypto.randomBytes(16).toString('base64');
    const hash = crypto.createHmac('sha512',salt).update(password).digest("base64");

    const cipherPassword = salt + "$" + hash;

    const user = {
        name: name,
        email: email,
        cipherPassword: cipherPassword,
        active: true,
    }

    dbUser=await userModel.create(user);
    dbUser=dbUser.toObject();

    return dbUser;
}


exports.getUserByEmailPasswordProject = async (email, project, password) => {

    let dbUser = await getUserWithRolesByEmailProject(email, project);
    if (dbUser!==null) {
        const cipherPasswordParts = dbUser.cipherPassword.split("$");
        hash = crypto.createHmac('sha512',cipherPasswordParts[0]).update(password).digest("base64");
        delete dbUser.cipherPassword;
        if (hash!==cipherPasswordParts[1]) {
            dbUser=null;
        }
    }
    return dbUser;
}

exports.getUserByIdProject = async (id, project) => {

    let dbUser = await getUserWithRolesByIdProject(id, project);
    if (dbUser!==null) {
        delete dbUser.cipherPassword;
    }
    return dbUser;
}

exports.addRolesForProject = async (project, email, roles) => {    

    const dbUser = await getUserByEmail(email);
    const query = { $and: [{userId: dbUser._id}, {project: project}]};
    const update = { $addToSet: { roles: {$each:roles}}};
    const options = { upsert: true };
    await userRoleModel.updateOne(query, update, options);
}

exports.removeRolesForProject = async (project, email, roles) => {    

    const dbUser = await getUserByEmail(email);
    const query = { $and: [{userId: dbUser._id}, {project: project}]};
    const update = { $pull: { roles: {$in:roles}}};
    const options = {};
    await userRoleModel.updateOne(query, update, options);
}