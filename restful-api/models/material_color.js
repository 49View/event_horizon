var mongoose = require("mongoose");
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model(
  "material_colors",
  new Schema({}, { strict: false })
);
