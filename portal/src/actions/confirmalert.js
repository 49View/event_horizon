import { SET_CONFIRM_ALERT, REMOVE_CONFIRM_ALERT } from "./types";
import uuid from "uuid";

export const showConfirmAlert = (msg, alertType) => dispatch => {
  const id = uuid.v4();

  dispatch({
    type: SET_CONFIRM_ALERT,
    payload: { msg, alertType, id }
  });

  setTimeout(() => dispatch({ type: REMOVE_CONFIRM_ALERT, payload: id }), 5000);
};

export const removeConfirmAlert = () => dispatch => {
  dispatch({
    type: REMOVE_CONFIRM_ALERT,
    payload: {}
  });
};
