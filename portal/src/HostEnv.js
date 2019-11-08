import axios from "axios";

let apiHostName = "api.ateventhorizon";

export const initHostEnv = () => {
  if (process.env.NODE_ENV && process.env.NODE_ENV === "development") {
    // dev code
    axios.defaults.baseURL = "https://localhost:3000";
    apiHostName = "localhost";
  } else {
    // production code
    axios.defaults.baseURL = "https://api.ateventhorizon.com";
    apiHostName = "api.ateventhorizon";
  }
  axios.defaults.withCredentials = true;

  if (
    process.env.REACT_APP_USE_API &&
    process.env.REACT_APP_USE_API === "production"
  ) {
    axios.defaults.baseURL = "https://api.ateventhorizon.com";
    apiHostName = "api.ateventhorizon";
  }
};

export const getApiHostName = () => apiHostName;
