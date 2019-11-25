import axios from "axios";

export const initHostEnv = () => {
  axios.defaults.withCredentials = true;
};
