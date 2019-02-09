const webSocket = require('ws');
const queryString = require('query-string');
const sessionController = require('../controllers/sessionController');
const userController = require('../controllers/userController');

let wsServer = null;
let sendPingHandler = null

const noop = () => {};

const onSocketServerSendPing = () => {

    this.wsServer.clients.forEach( client => {
        //console.log((new Date())+" - Sending PING to ", client.session._id);
        if (client.isAlive===false) {
            client.terminate();
        } else {
            client.isAlive=false;
            client.ping(noop);
        }
    });
}

const checkSessionById = async (sessionId, ipAddress, userAgent) => {
    const session = await sessionController.getValidSessionById(sessionId);
    if (session!==null) {
        const user =await userController.getUserByIdProject(session.userId, session.project);    

        session.user = user;
    }

    return session;
}

const onSocketServerConnection = async (client, req) => {

    const ipAddress = req.client.remoteAddress;
    const userAgent = req.headers['user-agent'];

    let {url, query} = queryString.parseUrl(req.url)
    const sessionId = query.s;

    session = await checkSessionById(sessionId, ipAddress, userAgent);
    if (session===null) {
        client.terminate();
        console.log((new Date()) + ' Connection rejected.');
    } else {
        //Save connection in session
        client.session = session;
        client.isAlive = true;
        console.log((new Date()) + ' Connection accepted.');
        client.ping(noop);
        console.log('Session connected: ',client.session);
        client.on('message', (message) => onSocketClientMessage(client,message));
        client.on('pong', () => onSocketClientHeartBeat(client));
        client.on('close', () => onSocketClientClose(client));
    }    
}

const onSocketClientClose = (client) => {
    console.log((new Date())+" - Client disconnected ", client.session._id)
}

const onSocketClientHeartBeat = (client) => {

    //console.log((new Date())+" - Receiving PONG ",client.session._id);
    client.isAlive=true;
}

const onSocketClientMessage = async(client, message) => {

    console.log((new Date())+" - Message received from: ",client.session._id);
    console.log("Message: "+message);
    try {
        let messageObject=JSON.parse(message);
        if (messageObject!==null) {
            if (messageObject.type==="refresh") {
                let s = messageObject.s;
                console.log(s);
                let session = await checkSessionById(s, null, null);
                console.log("S:",session);
                if (session!==null) {
                    console.log((new Date())+' - Session refreshed: ',session);
                    client.session=session;
                } else {
                    console.log((new Date())+' - Invalid Session - Client disconnected');
                    client.terminate();
                }
            }
        }
    } catch (err) {
        console.log("Invalid message", err);
    }
}

exports.createSocketServer = (server) => {
    
    this.wsServer = new webSocket.Server({
        server
    }); 
  
    this.wsServer.on('connection', onSocketServerConnection);
    this.sendPingHandler = setInterval(onSocketServerSendPing, 30000);
}

exports.sendMessageToAllClients = (message) => {
    console.log((new Date())+" - Send message to all client");
    this.wsServer.clients.forEach( client => {
        client.send(message);
    });
}
  
exports.sendMessageToSessionWithRole = (role, message) => {
    console.log((new Date())+" - Send message to client with session user role "+role);
    this.wsServer.clients.forEach( client => {
        if (client.session.user.roles.indexOf(role)>=0) {
            client.send(message);
        }
    });
}
  
exports.sendMessageToSessionWithUserId = (userId, message) => {
    console.log((new Date())+" - Send message to client with session userId "+userId);
    this.wsServer.clients.forEach( client => {
        if (client.session.user._id==userId) {
            client.send(message);
        }
    });
}
  
// const ws_send = ( _message ) => {
//     wsClients.forEach( function(client) {
//         client.sendUTF(_message);
//     });
// }

// exports.ping = ( req, res ) => {
//     const jr = { msg: 'ping', text: "ping" };
//     server.ws_send('message', JSON.stringify(jr) );
//     res.json( jr );
// }

// exports.send = ( req, res ) => {
//     const jr = req.body;
//     const jrs = JSON.stringify(jr);
//     console.log( "[WEB-SOCKET][MESSAGE] " + jrs )
//     ws_send( jrs );
//     res.json( jr );
// }
