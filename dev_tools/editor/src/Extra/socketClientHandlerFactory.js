import io from 'socket.io-client';

const socketClientHandlerFactory = () => {

    const socketMessageHandler = (message) => {
        const parsedMessage = JSON.parse(message);
        console.log("FROM SOCKET " , parsedMessage);
        if (parsedMessage.msg==='/HouseBSData/create') {
            //{“msg”:“/HouseBSData/create”,“name”:“5b7c8ad17ddedb00043f20f5"}
            console.log("NEW PROPERTY ADDED");
            // const propertyId = parsedMessage.name;
            // store.dispatch(actionCreators.updateLastPropertyLoaded(propertyId));
        }
    }

    return { 
        socket: null,
        // store: store,
        socketMessageHandler: socketMessageHandler,
        onSocketEventCallback: null,
        connect: (host) => {
            window.SocketClientHandler.onSocketEventCallback = (event, data) => { window.Module.onSocketEventCallback(event, JSON.stringify(data)) };
            window.SocketClientHandler.socket = io(host);
            window.SocketClientHandler.socket.on("connect", (socket) => {
                window.SocketClientHandler.onSocketEventCallback("connect", null); 
            });
            window.SocketClientHandler.socket.on("message", (message) => {
                window.SocketClientHandler.socketMessageHandler(message);
                window.SocketClientHandler.onSocketEventCallback("message", message);                                                
            });
            window.SocketClientHandler.socket.on("disconnect", (reason) => {
                window.SocketClientHandler.onSocketEventCallback("disconnect", { reason: reason});                            
            });
            window.SocketClientHandler.socket.on("error", (error) => {
                window.SocketClientHandler.onSocketEventCallback("error", error);                            
            });
        },
        send: (data) => {
            if (window.SocketClientHandler.socket!==null) {
                const jsonData = JSON.parse(data);
    
                window.SocketClientHandler.socket.emit("message", jsonData);
            }
        }
    }
    
}

export default socketClientHandlerFactory;
