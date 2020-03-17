var mongoose = require("mongoose");
// const uniqid = require('uniqid');
// const sha256 = require('sha256');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

// const generateUniqueId = () => {

//     const uid = sha256(uniqid()+"-"+uniqid()+"-"+uniqid()+"-"+uniqid());
//     console.log(uid);
//     return uid;
// }

module.exports = mongoose.model("sessions", new Schema({}, { strict: false }));
