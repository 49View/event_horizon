import { SET_EDITOR_CONTENT } from "../actions/types";

const initialState = {};

export default function(state = initialState, action) {
  const { type, payload } = action;

  switch (type) {
    case SET_EDITOR_CONTENT:
      return { content: payload };
    default:
      return state;
  }
}
