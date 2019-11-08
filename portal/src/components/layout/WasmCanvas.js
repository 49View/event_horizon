import React, { useState, useEffect } from "react";
import { useSelector, useDispatch } from "react-redux";
import axios from "axios";

const WASM_LOAD_START = "WASM_LOAD_START";
const WASM_LOAD_SUCCESS = "WASM_LOAD_SUCCESS";
const WASM_LOAD_FAILED = "WASM_LOAD_FAILED";
const WASM_RUN_SUCCESS = "WASM_RUN_SUCCESS";
const WASM_RUN_FAILED = "WASM_RUN_FAILED";
const WASM_SET_ROOT_CANVAS = "WASM_SET_ROOT_CANVAS";
const WASM_SET_CANVAS_SIZE = "WASM_SET_CANVAS_SIZE";
const WASM_RESIZE_CALLBACK = "WASM_RESIZE_CALLBACK";
const WASM_SET_CANVAS_VISIBILITY = "WASM_SET_CANVAS_VISIBILITY";
const WASM_ADD_CONSOLE_TEXT = "WASM_ADD_CONSOLE_TEXT";

const initialState = {
  error: null,
  resize: true,
  loading: false,
  loaded: false,
  running: false,
  wasmCanvas: null,
  wasmParams: null,
  consoleOutput: [],
  consoleOutputDirty: false,
  canvasWidth: 1,
  canvasHeight: 1,
  canvasTop: 0,
  canvasLeft: 0,
  canvasVisible: "hidden"
};

export const wasmSetCanvasVisibility = visible => {
  return {
    type: WASM_SET_CANVAS_VISIBILITY,
    visible: visible
  };
};

export const wasmSetCanvasSize = rect => {
  return {
    type: WASM_SET_CANVAS_SIZE,
    payload: rect
  };
};

const updateObject = (currentObject, updatedObject) => {
  return {
    ...currentObject,
    ...updatedObject
  };
};

export const wasmSetCanvasSizeCallback = () => {
  return {
    type: WASM_RESIZE_CALLBACK,
    payload: null
  };
};

export const setWasmLoaded = flag => dispatch => {
  dispatch({
    type: flag === true ? WASM_LOAD_SUCCESS : WASM_LOAD_FAILED
  });
};

export const loadWasmComplete = async (
  project,
  canvasRef,
  argumentList,
  dispatch
) => {
  try {
    if (!checkWasmSupport()) {
      throw new Error("Web assembly not supported");
    }
    if (!checkWebGL2Support()) {
      throw new Error("WebGl2 not supported");
    }

    let wasmAxios = axios.create();
    wasmAxios.defaults.baseURL = "";

    let prefolder = "";

    const currentDate = new Date();
    let downloadConfig = {
      url: prefolder + project + ".wasm?t=" + currentDate.getTime(),
      method: "get",
      responseType: "arraybuffer"
    };
    const binaryContent = await wasmAxios(downloadConfig);
    const wasmBinary = new Uint8Array(binaryContent.data);

    downloadConfig = {
      url: prefolder + project + ".js?t=" + currentDate.getTime(),
      method: "get",
      responseType: "text"
    };
    const content = await wasmAxios(downloadConfig);

    const wasmScript = content.data;
    window.wasmBinary = wasmBinary;
    window.wasmScript = wasmScript;
  } catch (ex) {
    console.log(ex);
  }

  window.addEventListener("resize", () => {
    dispatch(wasmSetCanvasSizeCallback());
    // console.log("Window Resize: ", window.innerWidth, window.innerWidth);
  });

  window.Module = {
    arguments: argumentList,
    print: text => {
      console.log("[WASM] " + text);
      if (!text.startsWith("[INFO]")) {
        dispatch({
          type: WASM_ADD_CONSOLE_TEXT,
          payload: text
        });
      }
    },
    printErr: text => {
      console.log("[WASM-ERROR] " + text);
    },
    canvas: canvasRef,
    onRuntimeInitialized: () => {
      console.log("Runtime initialized");
      // dispatch(wasmRunSuccess(canvas));
    },
    instantiateWasm: (imports, successCallback) => {
      WebAssembly.instantiate(window.wasmBinary, imports)
        .then(function(output) {
          console.log("wasm instantiation succeeded");
          successCallback(output.instance);
        })
        .catch(function(e) {
          console.log("wasm instantiation failed! " + e);
          // dispatch(wasmRunFailed(e.message));
        });
      return {};
    },
    destroy: cpp => {
      console.log("destroy");
    },
    doNotCaptureKeyboard: true
  };

  const s = document.createElement("script");
  s.text = window.wasmScript;
  document.body.appendChild(s);
};

const checkWasmSupport = () => {
  return typeof WebAssembly === "object";
};

const checkWebGL2Support = () => {
  let canvas = document.createElement("canvas");
  let result = false;
  try {
    if (canvas.getContext("webgl2") !== null) {
      result = true;
    }
  } catch (ex) {}
  return result;
};

const WasmCanvas = props => {
  let canvas = React.useRef(null);
  const [count, setCount] = useState(0);
  const dispatch = useDispatch();

  const wasmState = useSelector(state => state.wasm);
  const userData = useSelector(state => state.auth.userdata);
  const session = userData ? userData.session : null;

  useEffect(() => {
    if (count === 0 && session) {
      loadWasmComplete(
        props.wasmName,
        canvas.current,
        props.argumentList,
        dispatch
      );
      setCount(1);
    }
  }, [canvas, count, session, dispatch, props]);

  const canvasSizeX = wasmState.canvasWidth.toString() + "px";
  const canvasSizeY = wasmState.canvasHeight.toString() + "px";

  const canvasClientSizeX =
    (wasmState.canvasWidth * (window.devicePixelRatio || 1)).toString() + "px";
  const canvasClientSizeY =
    (wasmState.canvasHeight * (window.devicePixelRatio || 1)).toString() + "px";

  const canvasPadding = "1px";
  const canvasMargin = "0px";
  const canvasRadius = "5px";

  const canvasStyle = {
    position: "absolute",
    visibility: wasmState.canvasVisible,
    width: canvasSizeX,
    height: canvasSizeY,
    left: wasmState.canvasLeft,
    top: wasmState.canvasTop,
    margin: canvasMargin,
    padding: canvasPadding,
    borderRadius: canvasRadius
  };

  return (
    <canvas
      id="WasmCanvas"
      width={canvasClientSizeX}
      height={canvasClientSizeY}
      style={canvasStyle}
      ref={canvas}
      className="Canvas"
      onContextMenu={e => e.preventDefault()}
    />
  );
};

export default WasmCanvas;

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

const wasmSetCanvasSizeInternal = (state, rect) => {
  return updateObject(state, {
    canvasTop: rect.top,
    canvasLeft: rect.left,
    canvasWidth: rect.width,
    canvasHeight: rect.height,
    resize: false
  });
};

const wasmSetCanvasVisibilityInternal = (state, action) => {
  return updateObject(state, {
    canvasVisible: action.visible
  });
};

export const wasm_reducer = (state = initialState, action) => {
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
    case WASM_ADD_CONSOLE_TEXT:
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
      return wasmSetCanvasSizeInternal(state, action.payload);
    case WASM_SET_CANVAS_VISIBILITY:
      return wasmSetCanvasVisibilityInternal(state, action);
    default:
      return state;
  }
};
