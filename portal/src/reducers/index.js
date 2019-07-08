import { combineReducers } from "redux";
import alert from "./alert";
import confirmalert from "./confirmalert";
import auth from "./auth";
import entities from "./entities";

export default combineReducers({
  alert,
  confirmalert,
  auth,
  entities
});
