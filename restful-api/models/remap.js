var mongoose = require("mongoose");
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model("remaps", new Schema({}, { strict: false }));
