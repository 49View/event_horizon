import {w3cwebsocket as W3CWebSocket} from "websocket";
import store from "../store";
import {ADD_ENTITY, LOADING_FINISHED} from "../actions/types";
import {getFullEntity} from "../actions/entities";

let webSocketClient = null;

const wscSendInternal = (message, obj) => {
  if (webSocketClient.readyState === webSocketClient.OPEN) {
    const sd = {
      msg: message,
      data: obj
    };
    // console.log("[WSS] Sending: ", sd);
    webSocketClient.send(JSON.stringify(sd));
  }
};

export const updateAsset = (currentEntityData, currentEntity) => {
  try {
    const obj = {
      group: currentEntity.entity.group,
      entity_id: currentEntity.entity._id,
      data: currentEntityData
    };
    wscSend("UpdateEntity", obj);
  } catch (err) {
    console.log(err);
  }
};

export const placeHolderAsset = group => {
  try {
    const obj = {
      group: group
    };
    wscSend("AddPlaceHolderEntity", obj);
  } catch (err) {
    console.log(err);
  }
};

export const wscConnect = session => {
  const webSocketServerAddress = `wss://${process.env.REACT_APP_EH_CLOUD_HOST}/wss/?s=${session}`;

  webSocketClient = new W3CWebSocket(webSocketServerAddress);
  webSocketClient.onopen = () => {
    console.log("[WSS-REACT]WebSocket Client Connected");
  };
  webSocketClient.onmessage = message => {
    let mdata = JSON.parse(message.data);
    if ( mdata.msg.startsWith("EntityAdded") ) {
      const state = store.getState();
      const entity = mdata.data.fullDocument;
      if (state.entities.groupSelected === entity.group) {
          store.dispatch({type: ADD_ENTITY, payload: entity});
          store.dispatch(getFullEntity(entity));
      }
      store.dispatch({type: LOADING_FINISHED, payload: null});
    }
    // else if (mdata.msg === "materialsForGeom") {
    //   setEntityNodes(mdata.data);
    // }
  };
};

export const wscClose = user => {
  wscSendInternal("Logout", `${user.name} has logged out!`);
  webSocketClient.close();
};

export const wscSend = (message, obj) => {
  wscSendInternal(message, obj);
};
