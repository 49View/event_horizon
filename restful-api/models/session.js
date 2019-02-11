var mongoose = require('mongoose');
const uuidv4 = require('uuid/v4');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('sessions', new Schema({ _id: { type: String, default: uuidv4() }},{ "strict": false }));