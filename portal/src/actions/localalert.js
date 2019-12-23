import { SET_LOCAL_ALERT, REMOVE_LOCAL_ALERT } from "./types";
import uuid from "uuid";

  export const setLocalAlert = (msg, alertType, duration = 5000) => dispatch => {
  const id = uuid.v4();

  dispatch({
    type: SET_LOCAL_ALERT,
    payload: { msg, alertType, id }
  });

  setTimeout(
    () => dispatch({ type: REMOVE_LOCAL_ALERT, payload: id }),
    duration
  );
};
