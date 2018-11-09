var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('app_options', new Schema({},{ "strict": false }));