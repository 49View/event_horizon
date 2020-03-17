var mongoose = require("mongoose");
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model(
  "fs_source_assets.files",
  new Schema({}, { strict: false })
);
