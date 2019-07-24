import { w3cwebsocket as W3CWebSocket } from "websocket";
import store from "../store";
import { setEntityNodes } from "../actions/entities";

let webSocketClient = null;

const wscSendInternal = (message, obj) => {
  if (webSocketClient.readyState === webSocketClient.OPEN) {
    const sd = {
      msg: message,
      data: obj
    };
    console.log("[WSS] Sending: ", sd);
    webSocketClient.send(JSON.stringify(sd));
  }
};

export const requestAsset = currentEntity => {
  try {
    const obj = {
      group: currentEntity.entity.group,
      hash: currentEntity.entity.metadata.hash,
      entity_id: currentEntity.entity._id
    };
    wscSend("LoadGeomAndReset", obj);
  } catch (err) {
    console.log(err);
  }
};

export const wscConnect = session => {
  webSocketClient = new W3CWebSocket("wss://localhost:3000/?s=" + session);
  webSocketClient.onopen = () => {
    console.log("[WSS-REACT]WebSocket Client Connected");
  };
  webSocketClient.onmessage = message => {
    const mdata = JSON.parse(message.data);
    const state = store.getState();
    console.log("[WSS-REACT][MSGREC] ", mdata);
    if (state.entities.currentEntity) {
      if (mdata.msg === "requestAsset") {
        requestAsset(state.entities.currentEntity);
      }
    }
    if (mdata.msg === "materialsForGeom") {
      console.log("[WSS-REACT][MSGREC] ", mdata.data);
      setEntityNodes(mdata.data);
    }
    // console.log(message);
  };
};

export const wscClose = user => {
  wscSendInternal("Logout", `${user.name} has logged out!`);
  webSocketClient.close();
};

export const wscSend = (message, obj) => {
  wscSendInternal(message, obj);
};
