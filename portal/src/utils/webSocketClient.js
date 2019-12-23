import {w3cwebsocket as W3CWebSocket} from "websocket";
import store from "../store";
import {ADD_ENTITY} from "../actions/types";
import {getFullEntity, setEntityNodes, wasmClientFinishedLoadingData} from "../actions/entities";
import {setAlert} from "../actions/alert";

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

export const requestAsset = currentEntity => {
  try {
    const obj = {
      group: currentEntity.entity.group,
      hash: currentEntity.entity.hash,
      entity_id: currentEntity.entity._id
    };
    wscSend("LoadGeomAndReset", obj);
  } catch (err) {
    console.log(err);
  }
};

export const wscConnect = session => {
  let webSocketServerAddress = "wss://localhost/wss/?s=" + session;

  webSocketClient = new W3CWebSocket(webSocketServerAddress);
  webSocketClient.onopen = () => {
    console.log("[WSS-REACT]WebSocket Client Connected");
  };
  webSocketClient.onmessage = message => {
    let mdata = JSON.parse(message.data);
    // mdata.data = typeof mdata.data === "object" ? mdata.data : JSON.parse(mdata.data);
    // let mdata = message.data;
    const state = store.getState();
    console.log("[WSS-REACT][MSGREC] ", mdata);
    if (state.entities.currentEntity) {
      if (mdata.msg === "requestAsset") {
        requestAsset(state.entities.currentEntity);
      }
    }
    if (mdata.msg === "entityAdded") {
      store.dispatch(getFullEntity(mdata.data));
      store.dispatch({type: ADD_ENTITY, payload: mdata.data});
    } else if (mdata.msg === "wasmClientFinishedLoadingData") {
      store.dispatch(wasmClientFinishedLoadingData(mdata.data));
    } else if (mdata.msg === "materialsForGeom") {
      setEntityNodes(mdata.data);
    } else if (mdata.msg === "daemonLogger") {
      store.dispatch(setAlert(mdata.data.msg, mdata.data.type));
    }
  };
};

export const wscClose = user => {
  wscSendInternal("Logout", `${user.name} has logged out!`);
  webSocketClient.close();
};

export const wscSend = (message, obj) => {
  wscSendInternal(message, obj);
};
