var House = require('../models/house');

exports.house_index = function(req, res) {
async.parallel({
        house_count: function(callback) {
            House.count(callback);
        },
    }, function(err, results) {
        res.json(results);
    });
};

exports.house_delete_get = function(req, res) {
    res.send('NOT IMPLEMENTED: Author delete GET');
};
