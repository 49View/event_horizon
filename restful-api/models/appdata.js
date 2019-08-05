var mongoose = require("mongoose");
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model("appdata", new Schema({}, { strict: false }));
