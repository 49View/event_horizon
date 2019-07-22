import { w3cwebsocket as W3CWebSocket } from "websocket";
import store from "../store";
import { setEntityNodes } from "../actions/entities";
// import { SET_ENTITY_NODES } from "../actions/types";

let webSocketClient = null;

const wscSendInternal = (message, obj) => {
  const sd = {
    msg: message,
    data: obj
  };
  console.log("[WSS] Sending: ", sd);
  webSocketClient.send(JSON.stringify(sd));
};

export const wscConnect = session => {
  webSocketClient = new W3CWebSocket("wss://localhost:3000/?s=" + session);
  webSocketClient.onopen = () => {
    console.log("WebSocket Client Connected");
  };
  webSocketClient.onmessage = message => {
    const mdata = JSON.parse(message.data);
    const state = store.getState();
    console.log("[WSS-REACT][MSGREC] ", mdata);
    if (state.entities.currentEntity) {
      if (mdata.msg === "requestAsset") {
        const currentEntity = state.entities.currentEntity;
        const obj = {
          group: currentEntity.entity.group,
          hash: currentEntity.entity.metadata.hash,
          entity_id: currentEntity.entity._id
        };
        wscSendInternal("loadAsset", obj);
      }
    }
    if (mdata.msg === "materialsForGeom") {
      console.log("[WSS-REACT][MSGREC] ", mdata.data);
      // store.dispatch({
      //   type: SET_ENTITY_NODES,
      //   payload: mdata.data
      // });
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
