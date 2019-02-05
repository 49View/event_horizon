import axios from 'axios';
import * as actionTypes from './actionsTypes';

export const wasmRunSuccess = () => {
    return {
        type: actionTypes.WASM_RUN_SUCCESS
    }
}

export const wasmRunFailed = (error) => {
    return {
        type: actionTypes.WASM_RUN_FAILED,
        error: error
    }
}

export const runWasm = (canvas) => {
    return dispatch => {
        window.Module = {
            print: (text) => {
                console.log("W: "+text);
            },
            printErr: (text) => {
                console.log("W ERROR: "+text);
            },
            canvas: canvas,
            onRuntimeInitialized: () => {
                console.log("Runtime initialized");
                dispatch(wasmRunSuccess());
            },
            instantiateWasm: (imports, successCallback) => {
                WebAssembly.instantiate(window.wasmBinary, imports).then(function(output) {
                    console.log('wasm instantiation succeeded');
                    successCallback(output.instance);
                }).catch(function(e) {
                    console.log('wasm instantiation failed! ' + e);
                    dispatch(wasmRunFailed(e.message));
                });
                return {};
            }
        };

        const s = document.createElement('script');                        
        s.text = window.wasmScript;
        document.body.appendChild(s);         
    }
}

export const wasmLoadStart = () => {
    return {
        type: actionTypes.WASM_LOAD_START
    }
}

export const wasmLoadSuccess = (wasmBinary, wasmScript) => {
    return {
        type: actionTypes.WASM_LOAD_SUCCESS,
        wasmBinary: wasmBinary,
        wasmScript: wasmScript
    }
}

export const wasmLoadFailed = (error) => {
    return {
        type: actionTypes.WASM_LOAD_FAILED,
        error: error
    }
}

export const loadWasm = (project) => {
    return wrap(async (dispatch) => {
        dispatch(wasmLoadStart());
        try {
            if (!checkWasmSupport()) {
                throw new Error("Web assembly not supported");
            }
            if (!checkWebGL2Support()) {
                throw new Error("WebGl2 not supported");
            }

            const currentDate=new Date();
            let downloadConfig = {
                url: project+".wasm?t="+currentDate.getTime(),
                method: "get",
                responseType: "arraybuffer"
            }
            const binaryContent = await axios(downloadConfig);
            const wasmBinary=new Uint8Array(binaryContent.data);

            downloadConfig = {
                url: project+".js?t="+currentDate.getTime(),
                method: "get",
                responseType: "text"
            }
            const content = await axios(downloadConfig);
            const wasmScript= content.data;
            window.wasmBinary=wasmBinary;
            window.wasmScript=wasmScript;
            dispatch(wasmLoadSuccess());
        } catch (ex) {
            console.log(ex);
            dispatch(wasmLoadFailed(ex.message));

        }    
    })       
}

const checkWasmSupport = () => {
    return (typeof WebAssembly === 'object');
}

const checkWebGL2Support = () => {
    let canvas=document.createElement('canvas');
    let result=false;
    try {
        if (canvas.getContext("webgl2")!==null) {
            result=true;
        }
    } catch (ex) {

    }
    return result;
}

const  wrap = (fn) => {
    return function(dispatch) {
        fn(dispatch).catch(error => dispatch({ type: 'ERROR', error }));
    };
}

