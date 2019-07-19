import axios from "axios";
import {
  WASM_LOAD_START,
  WASM_LOAD_SUCCESS,
  WASM_LOAD_FAILED,
  WASM_RUN_SUCCESS,
  WASM_RUN_FAILED
} from "./types";

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

export const reCanvas = canvas => {
  window.Module["canvas"] = canvas;
};

export const runWasm = (canvas, token) => {
  return dispatch => {
    console.log("Running wasm start");

    window.Module = {
      arguments: [token],
      print: text => {
        console.log("W: " + text);
      },
      printErr: text => {
        console.log("W ERROR: " + text);
      },
      canvas: canvas,
      onRuntimeInitialized: () => {
        console.log("Runtime initialized");
        dispatch(wasmRunSuccess(canvas));
      },
      instantiateWasm: (imports, successCallback) => {
        WebAssembly.instantiate(window.wasmBinary, imports)
          .then(function(output) {
            console.log("wasm instantiation succeeded");
            successCallback(output.instance);
          })
          .catch(function(e) {
            console.log("wasm instantiation failed! " + e);
            dispatch(wasmRunFailed(e.message));
          });
        return {};
      },
      destroy: cpp => {
        console.log("destroy");
      }
    };

    const s = document.createElement("script");
    s.text = window.wasmScript;
    document.body.appendChild(s);
  };
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
  return wrap(async dispatch => {
    dispatch(wasmLoadStart());
    try {
      if (!checkWasmSupport()) {
        throw new Error("Web assembly not supported");
      }
      if (!checkWebGL2Support()) {
        throw new Error("WebGl2 not supported");
      }

      const currentDate = new Date();
      let downloadConfig = {
        url: project + ".wasm?t=" + currentDate.getTime(),
        method: "get",
        responseType: "arraybuffer"
      };
      const binaryContent = await axios(downloadConfig);
      const wasmBinary = new Uint8Array(binaryContent.data);

      downloadConfig = {
        url: project + ".js?t=" + currentDate.getTime(),
        method: "get",
        responseType: "text"
      };
      const content = await axios(downloadConfig);

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
