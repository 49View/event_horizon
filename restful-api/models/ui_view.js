var mongoose = require('mongoose');
mongoose.Promise = Promise;
var autopopulate = require('mongoose-autopopulate');
var Schema = mongoose.Schema;

var ui_view_schema = Schema (
{
  "strict": false,
  name: String,
  size: [Number],
  margins: [Number],
  padding: [Number],
  arrangements: [{
    layout: { 
    	type: Schema.Types.ObjectId,
        ref: 'ui_layout',
        autopopulate: true
    },
    name: String,
    position: String
  }]  	
});

ui_view_schema.plugin(autopopulate);

module.exports = mongoose.model('ui_view', ui_view_schema);
