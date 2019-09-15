import {
  WASM_LOAD_START,
  WASM_LOAD_SUCCESS,
  WASM_LOAD_FAILED,
  WASM_RUN_SUCCESS,
  WASM_RUN_FAILED,
  WASM_SET_ROOT_CANVAS,
  ADD_CONSOLE_TEXT,
  WASM_SET_CANVAS_SIZE,
  WASM_RESIZE_CALLBACK,
  WASM_SET_CANVAS_VISIBILITY
} from "../actions/types";
import { updateObject } from "../utils/wasmUtils";

const initialState = {
  error: null,
  resize: true,
  loading: false,
  loaded: false,
  running: false,
  wasmCanvas: null,
  consoleOutput: [],
  consoleOutputDirty: false,
  canvasWidth: 100,
  canvasHeight: 100,
  canvasTop: 0,
  canvasLeft: 0,
  canvasVisible: true
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

const wasmSetCanvasSize = (state, rect) => {
  console.log("update canvas with ", rect);
  return updateObject(state, {
    canvasTop: rect.top,
    canvasLeft: rect.left,
    canvasWidth: rect.width,
    canvasHeight: rect.height,
    resize: false
  });
};

const wasmSetCanvasVisibility = (state, action) => {
  return updateObject(state, {
    canvasVisible: action.visible
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
    case ADD_CONSOLE_TEXT:
      return {
        ...state,
        consoleOutput: [...state.consoleOutput, action.payload],
        consoleOutputDirty: !state.consoleOutputDirty
      };
    case WASM_SET_ROOT_CANVAS:
      return {
        ...state,
        wasmCanvas: action.payload_canvas
      };
    case WASM_RESIZE_CALLBACK:
      return {
        ...state,
        resize: true
      };
    case WASM_SET_CANVAS_SIZE:
      return wasmSetCanvasSize(state, action.payload);
    case WASM_SET_CANVAS_VISIBILITY:
      return wasmSetCanvasVisibility(state, action);
    default:
      return state;
  }
};

export default reducer;
