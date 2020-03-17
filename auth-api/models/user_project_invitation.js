var mongoose = require("mongoose");
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model(
  "users_project_invitations",
  new Schema({}, { strict: false })
);
