import { SET_CONFIRM_ALERT, REMOVE_CONFIRM_ALERT } from "../actions/types";

const initialState = {};

export default function(state = initialState, action) {
  const { type, payload } = action;

  switch (type) {
    case SET_CONFIRM_ALERT:
      return { alert: payload };
    case REMOVE_CONFIRM_ALERT:
      return { alert: null };
    default:
      return state;
  }
}
