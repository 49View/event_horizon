import { combineReducers } from "redux";
import alert from "./alert";
import localalert from "./localalert";
import confirmalert from "./confirmalert";
import auth from "./auth";
import { wasmReducer as wasm } from "react-wasm-canvas";
import codemirror from "./codemirror";
import entities from "./entities";

export default combineReducers({
  alert,
  localalert,
  confirmalert,
  auth,
  wasm,
  codemirror,
  entities
});
