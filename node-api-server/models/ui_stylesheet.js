var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('ui_stylesheet', new Schema({},{ "strict": false }));