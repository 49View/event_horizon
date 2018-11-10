const crypto = require('crypto');
const userModel = require('../models/user');
const userRoleModel = require('../models/user_role');

const getUserByEmail = async (email) => {
    
    let dbUser = null;
    const query = {"email": email};

    dbUser=await userModel.findOne(query);
    dbUser=dbUser.toObject();

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

    const dbUser = await getUserByEmail(email);
    if (dbUser!==null) {
        const cipherPasswordParts = dbUser.cipherPassword.split("$");
        hash = crypto.createHmac('sha512',cipherPasswordParts[0]).update(password).digest("base64");
        if (hash!==cipherPasswordParts[1]) {
        }
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