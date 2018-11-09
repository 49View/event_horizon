
var server = require('../bin/www');

exports.ping = ( req, res ) => {
    const jr = { msg: 'ping', text: "ping" };
    server.sio.emit('message', JSON.stringify(jr) );
    res.json( jr );
}

exports.send = ( req, res ) => {
    const jr = req.body;
    const jrs = JSON.stringify(jr);
    console.log( "[WEB-SOCKET][MESSAGE] " + jrs )
    server.sio.emit('message', jrs );
    res.json( jr );
}
