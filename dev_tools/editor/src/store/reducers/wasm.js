import * as actionTypes from '../actions/actionsTypes';
import { updateObject} from '../utility';

const initialState = {
    error: null,
    loading: false,
    loaded: false,
    running: false
}

const wasmLoadStart = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: true
        });
}

const wasmLoadSuccess = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: false,
            loaded: true
        });
}

const wasmLoadFailed = (state,action) => {
    return updateObject(
        state, {
            error: action.error,
            loading: false,
        });
}

const wasmRunSuccess = (state,action) => {
    return updateObject(
        state, {
            error: null,
            running: true
        });
}

const wasmRunFailed = (state,action) => {
    return updateObject(
        state, {
            error: action.error
        });
}

const reducer = (state=initialState, action) => {

    switch (action.type) {
        case actionTypes.WASM_LOAD_START: return wasmLoadStart(state,action);
        case actionTypes.WASM_LOAD_SUCCESS: return wasmLoadSuccess(state,action);
        case actionTypes.WASM_LOAD_FAILED: return wasmLoadFailed(state,action);
        case actionTypes.WASM_RUN_SUCCESS: return wasmRunSuccess(state,action);
        case actionTypes.WASM_RUN_FAILED: return wasmRunFailed(state,action);
        default: return state;
    }
}

export default reducer;

