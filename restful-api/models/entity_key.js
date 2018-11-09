var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('entities_keys', new Schema({},{ "strict": false }));