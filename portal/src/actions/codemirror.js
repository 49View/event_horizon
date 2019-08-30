import { SET_EDITOR_CONTENT } from "./types";

export const setContent = value => dispatch => {
  dispatch({
    type: SET_EDITOR_CONTENT,
    payload: value
  });
};
