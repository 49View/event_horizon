import { setAlert } from "./alert";
import {
  REGISTER_SUCCESS,
  REGISTER_FAIL,
  USER_LOADED,
  AUTH_ERROR,
  LOGIN_SUCCESS,
  LOGIN_FAIL,
  LOGOUT
} from "./types";
import setAuthToken from "../utils/setAuthToken";
import axios from "axios";

// Load User
export const loadUser = () => async dispatch => {
  if (localStorage.token === undefined) return {};

  setAuthToken(localStorage.token);

  try {
    const res = await axios.get("/user");

    dispatch({
      type: USER_LOADED,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: AUTH_ERROR
    });
  }
};

// Register User
export const register = ({ name, email, password }) => async dispatch => {
  const config = {
    headers: {
      "Content-Type": "application/json"
    }
  };

  const body = JSON.stringify({ name, email, password });

  try {
    const res = await axios.post("/createuser", body, config);

    dispatch({
      type: REGISTER_SUCCESS,
      payload: res.data
    });

    dispatch(loadUser());
  } catch (err) {
    const errors = err.response.data.errors;

    if (errors) {
      errors.forEach(error => dispatch(setAlert(error.msg, "danger")));
    }

    dispatch({
      type: REGISTER_FAIL
    });
  }
};

// Login User
export const login = (email, password, project) => async dispatch => {
  const config = {
    headers: {
      "Content-Type": "application/json"
    }
  };

  let body = JSON.stringify({ email, password, project });
  try {
    let res = await axios.post("/gettoken", body, config);

    if (project === null || project.length === 0) {
      // Make sure we re-login with project set, otherwise most of the entity rest api req will fail
      project = res.data.project;
      body = JSON.stringify({ email, password, project });
      res = await axios.post("/gettoken", body, config);
    }

    dispatch({
      type: LOGIN_SUCCESS,
      payload: res.data
    });

    dispatch(loadUser());
  } catch (err) {
    const errors = err.errors;

    if (errors) {
      errors.forEach(error => dispatch(setAlert(error.msg, "danger")));
    }

    dispatch({
      type: LOGIN_FAIL
    });
    dispatch(setAlert("Login Failed", "danger"));
  }
};

// Logout / Clear Profile
export const logout = () => dispatch => {
  dispatch({ type: LOGOUT });
};
