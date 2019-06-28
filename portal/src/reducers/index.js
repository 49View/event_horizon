import { combineReducers } from "redux";
import alert from "./alert";
import auth from "./auth";
import entities from "./entities";

export default combineReducers({
  alert,
  auth,
  entities
});
