import { setAlert } from "./alert";
import { setLocalAlert } from "./localalert";
import {
  REGISTER_SUCCESS,
  REGISTER_FAIL,
  USER_LOADED,
  AUTH_ERROR,
  CLEAR_ENTITIES,
  LOGOFF_FROM_PROJECT,
  LOGIN_SUCCESS,
  LOGIN_FAIL,
  LOGOUT
} from "./types";
import axios from "axios";

// Load User
export const loadUser = () => async dispatch => {
  // if (localStorage.token === undefined) return {};

  try {
    const res = await axios.get(`/api/user`);

    dispatch({
      type: USER_LOADED,
      payload: res.data
    });
  } catch (err) {
    console.log(err);
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
    const res = await axios.post("/api/createuser", body, config);

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
    let res = await axios.post("/api/gettoken", body, config);

    // if (project === null || project.length === 0) {
    //   // Make sure we re-login with project set, otherwise most of the entity rest api req will fail
    //   project = res.data.project;
    //   body = JSON.stringify({ email, password, project });
    //   res = await axios.post("/api/gettoken", body, config);
    // }

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
export const logout = () => async dispatch => {
  try {
    await axios.get(`/api/cleanToken`);
    dispatch({ type: LOGOUT });
  } catch (error) {
    dispatch({
      type: LOGIN_FAIL
    });
    dispatch(setAlert("Logout Failed", "danger"));
  }
};

export const logoffFromProject = () => dispatch => {
  dispatch({ type: LOGOFF_FROM_PROJECT });
  dispatch({ type: CLEAR_ENTITIES });
};

// Create Project
export const createProject = projectName => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };

    console.log("Create user Project post ", projectName);
    await axios.post("/api/user/createProject/" + projectName);

    // Make sure we re-login with project set, otherwise most of the entity rest api req will fail
    const res = await axios.post("/api/refreshtoken/" + projectName, {}, config);

    dispatch({
      type: LOGIN_SUCCESS,
      payload: res.data
    });

    dispatch(loadUser());
  } catch (err) {
    console.log("craete project error: ", err);
    const errors = err.errors;

    if (errors) {
      errors.forEach(error => dispatch(setAlert(error.msg, "danger")));
    }

    dispatch(setAlert("Creating New Project Failed", "danger"));
  }
};

export const acceptInvitation = (projectName, userEmail) => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };

    const body = {
      email: userEmail,
      roles: ["user"]
    };
    await axios.put(
      "/api/user/addRolesFor/" + projectName,
      JSON.stringify(body),
      config
    );

    // delete invitation as last step so we are sure if something goes wrong the invitation is still on
    const bodyDelete = {
      persontoadd: userEmail,
      project: projectName
    };
    await axios.delete("user/invitetoproject/", bodyDelete, config);

    // dispatch({
    //   type: LOGIN_SUCCESS,
    //   payload: res.data
    // });

    dispatch(loadUser());
  } catch (err) {
    dispatch(setAlert("Cannot join project, investigating why...", "danger"));
  }
};

export const declineInvitation = (projectName, userEmail) => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };

    const bodyDelete = {
      persontoadd: userEmail,
      project: projectName
    };
    await axios.delete(
      "user/invitetoproject/",
      JSON.stringify(bodyDelete),
      config
    );

    // dispatch({
    //   type: LOGIN_SUCCESS,
    //   payload: res.data
    // });

    dispatch(loadUser());
  } catch (err) {
    dispatch(
      setAlert(
        "Cannot remove invitation to project, investigating why...",
        "danger"
      )
    );
  }
};

export const setCurrentProject = projectName => async dispatch => {
  try {
    // Make sure we re-login with project set, otherwise most of the entity rest api req will fail
    const res = await axios.post("/api/refreshtoken/" + projectName);

    dispatch({
      type: LOGIN_SUCCESS,
      payload: res.data
    });

    dispatch(loadUser());
  } catch (err) {
    console.log("craete project error: ", err);
    dispatch({
      type: LOGIN_FAIL
    });
    dispatch(setAlert("Cannot login to new project", "danger"));
  }
};

export const sendInvitationToProject = (
  adminuser,
  project,
  personToAdd
) => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };

    const body = {
      adminuser: adminuser,
      project: project,
      persontoadd: personToAdd
    };

    const res = await axios.put("/api/user/invitetoproject", body, config);
    console.log(res);
    dispatch(
      setLocalAlert(res.data.msg, res.data.code === 200 ? "success" : "danger")
    );
  } catch (ex) {
    dispatch(
      setLocalAlert("Server responded 400, it means it's bonker :'(", "danger")
    );
  }
};
