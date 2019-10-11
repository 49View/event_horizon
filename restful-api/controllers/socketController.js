const webSocket = require("ws");
const queryString = require("query-string");
const sessionController = require("../controllers/sessionController");
const userController = require("../controllers/userController");

let wsServer = null;
let sendPingHandler = null;

const noop = () => {};

const onSocketServerSendPing = () => {
  this.wsServer.clients.forEach(client => {
    //console.log((new Date())+" - Sending PING to ", client.session._id);
    if (client.isAlive === false) {
      client.terminate();
    } else {
      client.isAlive = false;
      client.ping(noop);
    }
  });
};

const checkSessionById = async (sessionId, ipAddress, userAgent) => {
  const session = await sessionController.getValidSessionById(sessionId);
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

  let { url, query } = queryString.parseUrl(req.url);
  const sessionId = query.s;

  session = await checkSessionById(sessionId, ipAddress, userAgent);
  if (session === null) {
    client.terminate();
    console.log("[WSS] ", new Date() + " Connection rejected.");
  } else {
    //Save connection in session
    client.session = session;
    client.isAlive = true;
    client.ping(noop);
    console.log("[WSS] Session connected: ", client.session, " ", new Date());
    client.on("message", message => onSocketClientMessage(client, message));
    client.on("pong", () => onSocketClientHeartBeat(client));
    client.on("close", () => onSocketClientClose(client));
  }
};

const onSocketClientClose = client => {
  console.log(new Date() + " - Client disconnected ", client.session._id);
};

const onSocketClientHeartBeat = client => {
  //console.log((new Date())+" - Receiving PONG ",client.session._id);
  client.isAlive = true;
};

const onSocketClientMessage = async (client, message) => {
  console.log(
    "[WSS]['msg'] {",
    client.session.user.name,
    "} ",
    message.slice(0, 50),
    " ...(truncated)"
  );
  // console.log("Message: "+message);
  try {
    //   let messageObject=JSON.parse(message);
    this.wsServer.clients.forEach(function each(oclient) {
      if (client !== oclient && oclient.readyState === webSocket.OPEN) {
        oclient.send(message);
      }
    });
  } catch (err) {
    console.log("[WSS][ERROR] Error on send message ", err);
  }
};

exports.createSocketServer = server => {
  this.wsServer = new webSocket.Server({
    server
  });

  this.wsServer.on("connection", onSocketServerConnection);
  this.sendPingHandler = setInterval(onSocketServerSendPing, 30000);
};

exports.sendMessageToAllClients = message => {
  console.log(new Date() + " - Send message to all client");
  this.wsServer.clients.forEach(client => {
    console.log("Client session ", client.session);
    client.send(message);
  });
};

exports.sendMessageToSessionWithRole = (role, message) => {
  console.log(
    new Date() + " - Send message to client with session user role " + role
  );
  this.wsServer.clients.forEach(client => {
    if (client.session.user.roles.indexOf(role) >= 0) {
      client.send(message);
    }
  });
};

exports.sendMessageToSessionWithUserId = (userId, message) => {
  console.log(
    new Date() + " - Send message to client with session userId " + userId
  );
  this.wsServer.clients.forEach(client => {
    if (client.session.userId == userId) {
      client.send(message);
    }
  });
};

exports.sendMessageToSessionWithUserIdProject = (userId, project, message) => {
  console.log(
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
      if (client.session._id.toString() === previousSessionId.toString()) {
        console.log(
          new Date() +
            " - Replace session " +
            previousSessionId +
            " with session " +
            currentSessionId
        );
        client.session = session;
      }
    });
  }
};

exports.closeClientsWithSessionId = sessionId => {
  this.wsServer.clients.forEach(client => {
    if (client.session._id.toString() === sessionId.toString()) {
      console.log(new Date() + " - Close session " + sessionId);
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
//     console.log( "[WEB-SOCKET][MESSAGE] " + jrs )
//     ws_send( jrs );
//     res.json( jr );
// }
