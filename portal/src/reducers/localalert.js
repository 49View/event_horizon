import { SET_LOCAL_ALERT, REMOVE_LOCAL_ALERT } from "../actions/types";

const initialState = [];

export default function(state = initialState, action) {
  const { type, payload } = action;

  switch (type) {
    case SET_LOCAL_ALERT:
      return [...state, payload];
    case REMOVE_LOCAL_ALERT:
      return state.filter(alert => alert.id !== payload);
    default:
      return state;
  }
}
