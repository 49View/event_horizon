import {SET_ALERT, REMOVE_ALERT, ENTITY_ERROR} from "./types";
import uuid from "uuid";

export const setAlert = (msg, alertType, duration = 0) => dispatch => {
  const id = uuid.v4();

  dispatch({
    type: SET_ALERT,
    payload: { msg, alertType, id }
  });

  dispatch({
    type: ENTITY_ERROR,
    payload: id
  });

  if ( duration > 0 ) {
    setTimeout(() => dispatch({ type: REMOVE_ALERT, payload: id }), duration);
  }
};
