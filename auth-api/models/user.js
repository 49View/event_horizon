var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('users', new Schema({},{ "strict": false }));