import {
  WASM_LOAD_START,
  WASM_LOAD_SUCCESS,
  WASM_LOAD_FAILED,
  WASM_RUN_SUCCESS,
  WASM_RUN_FAILED,
  WASM_SET_ROOT_CANVAS
} from "../actions/types";
import { updateObject } from "../utils/wasmUtils";

const initialState = {
  error: null,
  loading: false,
  loaded: false,
  running: false,
  wasmCanvas: null
};

const wasmLoadStart = (state, action) => {
  return updateObject(state, {
    error: null,
    loading: true
  });
};

const wasmLoadSuccess = (state, action) => {
  return updateObject(state, {
    error: null,
    loading: false,
    loaded: true
  });
};

const wasmLoadFailed = (state, action) => {
  return updateObject(state, {
    error: action.error,
    loading: false
  });
};

const wasmRunSuccess = (state, action) => {
  return updateObject(state, {
    error: null,
    running: true
    // wasmCanvas: action.wasmCanvas
  });
};

const wasmRunFailed = (state, action) => {
  return updateObject(state, {
    error: action.error
  });
};

const reducer = (state = initialState, action) => {
  switch (action.type) {
    case WASM_LOAD_START:
      return wasmLoadStart(state, action);
    case WASM_LOAD_SUCCESS:
      return wasmLoadSuccess(state, action);
    case WASM_LOAD_FAILED:
      return wasmLoadFailed(state, action);
    case WASM_RUN_SUCCESS:
      return wasmRunSuccess(state, action);
    case WASM_RUN_FAILED:
      return wasmRunFailed(state, action);
    case WASM_SET_ROOT_CANVAS:
      console.log("circular");
      return {
        ...state,
        wasmCanvas: action.payload_canvas
      };
    default:
      return state;
  }
};

export default reducer;
