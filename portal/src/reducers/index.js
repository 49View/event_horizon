import { combineReducers } from "redux";
import alert from "./alert";
import confirmalert from "./confirmalert";
import auth from "./auth";
import wasm from "./wasm";
import entities from "./entities";

export default combineReducers({
  alert,
  confirmalert,
  auth,
  wasm,
  entities
});
