var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('geoms', new Schema({},{ "strict": false }));