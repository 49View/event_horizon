import axios from "axios";
import {
  WASM_LOAD_START,
  WASM_LOAD_SUCCESS,
  WASM_LOAD_FAILED,
  WASM_RUN_SUCCESS,
  WASM_RUN_FAILED,
  WASM_SET_ROOT_CANVAS,
  ADD_CONSOLE_TEXT,
  WASM_RESIZE_CALLBACK,
  WASM_SET_CANVAS_SIZE,
  WASM_SET_CANVAS_VISIBILITY
} from "./types";
import store from "../store";

export const wasmSetCanvasSize = rect => {
  return {
    type: WASM_SET_CANVAS_SIZE,
    payload: rect
  };
};

export const wasmSetCanvasSizeCallback = () => {
  return {
    type: WASM_RESIZE_CALLBACK,
    payload: null
  };
};

export const wasmSetCanvasVisibility = visible => {
  return {
    type: WASM_SET_CANVAS_VISIBILITY,
    visible: visible
  };
};

export const wasmRunSuccess = canvas => {
  return {
    type: WASM_RUN_SUCCESS
    // wasmCanvas: canvas
  };
};

export const wasmRunFailed = error => {
  return {
    type: WASM_RUN_FAILED,
    error: error
  };
};

export const setMainCanvas = canvas => dispatch => {
  dispatch({
    type: WASM_SET_ROOT_CANVAS,
    payload_canvas: canvas
  });
};

export const setWasmLoaded = flag => dispatch => {
  dispatch({
    type: flag === true ? WASM_LOAD_SUCCESS : WASM_LOAD_FAILED
  });
};

export const reCanvas = canvas => {
  window.Module["canvas"] = canvas;
};

export const wasmLoadStart = () => {
  return {
    type: WASM_LOAD_START
  };
};

export const wasmLoadSuccess = (wasmBinary, wasmScript) => {
  return {
    type: WASM_LOAD_SUCCESS,
    wasmBinary: wasmBinary,
    wasmScript: wasmScript
  };
};

export const wasmLoadFailed = error => {
  return {
    type: WASM_LOAD_FAILED,
    error: error
  };
};

export const loadWasm = project => {
  console.log("load wasm");
  return wrap(async dispatch => {
    dispatch(wasmLoadStart());
    try {
      if (!checkWasmSupport()) {
        throw new Error("Web assembly not supported");
      }
      if (!checkWebGL2Support()) {
        throw new Error("WebGl2 not supported");
      }

      let wasmAxios = axios.create();
      wasmAxios.defaults.baseURL = "";

      const currentDate = new Date();
      let downloadConfig = {
        url: project + ".wasm?t=" + currentDate.getTime(),
        method: "get",
        responseType: "arraybuffer"
      };
      const binaryContent = await wasmAxios(downloadConfig);
      const wasmBinary = new Uint8Array(binaryContent.data);

      downloadConfig = {
        url: project + ".js?t=" + currentDate.getTime(),
        method: "get",
        responseType: "text"
      };
      const content = await wasmAxios(downloadConfig);

      const wasmScript = content.data;
      window.wasmBinary = wasmBinary;
      window.wasmScript = wasmScript;

      dispatch(wasmLoadSuccess());
    } catch (ex) {
      console.log(ex);
      dispatch(wasmLoadFailed(ex.message));
    }
  });
};

export const loadWasmComplete = async (
  project,
  canvasRef,
  userToken,
  userSessionId
) => {
  //async dispatch =>
  //async dispatch =>
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
    // if (!process.env.NODE_ENV || process.env.NODE_ENV === "development") {
    //   // dev code
    //   prefolder = "local/";
    // }

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
    store.dispatch(wasmSetCanvasSizeCallback());
    console.log("Window Resize: ", window.innerWidth, window.innerWidth);
  });

  window.Module = {
    arguments: [
      // userToken,
      userSessionId,
      (window.devicePixelRatio || 1).toString()
    ],
    print: text => {
      console.log("W: " + text);
      if (!text.startsWith("[INFO]")) {
        store.dispatch({
          type: ADD_CONSOLE_TEXT,
          payload: text
        });
      }
    },
    printErr: text => {
      console.log("W ERROR: " + text);
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

const wrap = fn => {
  return function(dispatch) {
    fn(dispatch).catch(error => dispatch({ type: "ERROR", error }));
  };
};
