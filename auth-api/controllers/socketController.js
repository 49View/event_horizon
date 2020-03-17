const webSocket = require("ws");
const queryString = require("query-string");
const sessionController = require("../controllers/sessionController");
const userController = require("../controllers/userController");
const logger = require("../logger");

let wsServer = null;
let sendPingHandler = null;

const noop = () => {
};

const onSocketServerSendPing = () => {
    this.wsServer.clients.forEach(client => {
        //logger.info((new Date())+" - Sending PING to ", client.session._id);
        if (client.isAlive === false) {
            client.terminate();
        } else {
            client.isAlive = false;
            client.ping(noop);
        }
    });
};

const checkSessionById = async (sessionId, ipAddress, userAgent) => {
    let session = await sessionController.getValidSessionById(sessionId);
    if (session === null) {
        const user = await userController.getUserByEmail("daemon");
        if (user) {
            session = {};
            session.user = user;
            session.userId = user._id;
        }
    }
    if (session !== null) {
        const user = await userController.getUserByIdProject(
            session.userId,
            session.project
        );
        session.user = user;
    }

    return session;
};

const onSocketServerConnection = async (client, req) => {
    const ipAddress = req.client.remoteAddress;
    const userAgent = req.headers["user-agent"];

    let {url, query} = queryString.parseUrl(req.url);
    const sessionId = query.s;

    const session = await checkSessionById(sessionId, ipAddress, userAgent);
    logger.info("[WSS] new connection: " + ipAddress + " userAgent: " + userAgent);
    client.session = session;
    client.isAlive = true;
    client.ping(noop);
    client.on("message", message => onSocketClientMessage(client, message));
    client.on("pong", () => onSocketClientHeartBeat(client));
    client.on("close", () => onSocketClientClose(client));

    //
    // const session = await checkSessionById(sessionId, ipAddress, userAgent);
    // if (session === null) {
    //   client.terminate();
    //   logger.info("[WSS] "+ new Date() + " Connection rejected.");
    // } else {
    //   //Save connection in session
    //   client.session = session;
    //   client.isAlive = true;
    //   client.ping(noop);
    //   logger.info("[WSS] Session connected: " + client.session + new Date());
    //   client.on("message", message => onSocketClientMessage(client, message));
    //   client.on("pong", () => onSocketClientHeartBeat(client));
    //   client.on("close", () => onSocketClientClose(client));
    // }
};

const onSocketClientClose = client => {
    logger.info(new Date() + " - Client disconnected ");
};

const onSocketClientHeartBeat = client => {
    //logger.info((new Date())+" - Receiving PONG ",client.session._id);
    client.isAlive = true;
};

const onSocketClientMessage = async (client, message) => {
    // logger.info(
    //   "[WSS]['msg'] {",
    //   client.session.user ? client.session.user.name : client.session.userId,
    //   "} ",
    //   message.slice(0, 50),
    //   " ...(truncated)"
    // );
    try {
        console.log("client message: ", message );
        let messageSanitized = typeof message === 'string' ? JSON.parse(unescape(message)) : message;
        if (messageSanitized.data) {
            if ( typeof messageSanitized.data === 'string' ) messageSanitized.data =  JSON.parse(unescape(messageSanitized.data));
            if (messageSanitized.data._id) {
                messageSanitized.data._id = messageSanitized.data._id.$oid;
            }
            if (messageSanitized.data.fsid) {
                messageSanitized.data.fsid = messageSanitized.data.fsid.$oid;
            }
        }
        const messageAsString = JSON.stringify(messageSanitized);
        // logger.info("[WSS] Message: " + messageAsString);
        //   let messageObject=JSON.parse(message);
        this.wsServer.clients.forEach(function each(oclient) {
            if (client !== oclient && oclient.readyState === webSocket.OPEN) {
                oclient.send(messageAsString);
            }
        });
    } catch (err) {
        logger.info("[WSS][ERROR] Error on send message ", err);
    }
};

exports.createSocketServer = port => {
    this.wsServer = new webSocket.Server({"port": port});
    this.wsServer.on("connection", onSocketServerConnection);
    this.sendPingHandler = setInterval(onSocketServerSendPing, 30000);
    logger.info("Starting WSS on Port: " + port);
};

exports.sendMessageToAllClients = message => {
    logger.info(new Date() + " - Send message to all client");
    this.wsServer.clients.forEach(client => {
        logger.info("Client session ", client.session);
        client.send(message);
    });
};

exports.sendMessageToSessionWithRole = (role, message) => {
    logger.info(
        new Date() + " - Send message to client with session user role " + role
    );
    this.wsServer.clients.forEach(client => {
        if (client.session.user.roles.indexOf(role) >= 0) {
            client.send(message);
        }
    });
};

exports.sendMessageToSessionWithUserId = (userId, message) => {
    logger.info(
        new Date() + " - Send message to client with session userId " + userId
    );
    this.wsServer.clients.forEach(client => {
        if (client.session.userId === userId) {
            client.send(message);
        }
    });
};

exports.sendMessageToSessionWithUserIdProject = (userId, project, message) => {
    logger.info(
        new Date() +
        " - Send message to client with session userId/project " +
        userId +
        "/" +
        project
    );
    this.wsServer.clients.forEach(client => {
        if (
            client.session.userId === userId &&
            client.session.project === project
        ) {
            client.send(message);
        }
    });
};

exports.replaceClientsSession = async (previousSessionId, currentSessionId) => {
    let session = await checkSessionById(currentSessionId, null, null);
    if (session !== null) {
        this.wsServer.clients.forEach(client => {
            console.log(client)
            client.session = session;
            // if (client.session._id.toString() === previousSessionId.toString()) {
            //     logger.info(
            //         new Date() +
            //         " - Replace session " +
            //         previousSessionId +
            //         " with session " +
            //         currentSessionId
            //     );
            //     client.session = session;
            // }
        });
    }
};

exports.closeClientsWithSessionId = sessionId => {
    this.wsServer.clients.forEach(client => {
        if (client.session && client.session._id.toString() === sessionId.toString()) {
            logger.info(new Date() + " - Close session " + sessionId);
            client.terminate();
        }
    });
};

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
//     logger.info( "[WEB-SOCKET][MESSAGE] " + jrs )
//     ws_send( jrs );
//     res.json( jr );
// }
