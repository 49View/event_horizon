var mongoose = require('mongoose');
mongoose.Promise = Promise;

var Schema = mongoose.Schema;

module.exports = mongoose.model('floorplan_calculus', new Schema({},{ "strict": false }));