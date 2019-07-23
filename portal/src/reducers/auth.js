import {
  REGISTER_SUCCESS,
  REGISTER_FAIL,
  USER_LOADED,
  AUTH_ERROR,
  LOGIN_SUCCESS,
  LOGIN_FAIL,
  LOGOUT
} from "../actions/types";

import { wscConnect, wscClose } from "../utils/webSocketClient";

const initialState = {
  token: localStorage.getItem("token"),
  isAuthenticated: null,
  loading: true,
  userdata: null
};

export default function(state = initialState, action) {
  const { type, payload } = action;

  switch (type) {
    case USER_LOADED:
      wscConnect(payload.session);
      return {
        ...state,
        isAuthenticated: true,
        loading: false,
        userdata: payload
      };
    case REGISTER_SUCCESS:
    case LOGIN_SUCCESS:
      localStorage.setItem("token", payload.token);
      return {
        ...state,
        ...payload,
        isAuthenticated: true,
        loading: false
      };
    case REGISTER_FAIL:
    case AUTH_ERROR:
    case LOGIN_FAIL:
    case LOGOUT:
      localStorage.removeItem("token");
      if (state.userdata) wscClose(state.userdata.user);
      return {
        token: null,
        isAuthenticated: false,
        loading: false,
        project: null,
        userdata: null
      };
    default:
      return state;
  }
}
