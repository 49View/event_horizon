import * as actionTypes from '../actions/actionsTypes';
import { updateObject} from '../utility';

const initialState = {
    error: null,
    loading: false,
    isGuest: false,
    user: null
}

const authStart = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: true
        });
}

const authSuccess = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: false,
            isGuest: action.isGuest,
            user: action.user            
        });
}

const authFail = (state,action) => {
    return updateObject(
        state, {
            error: action.error,
            loading: false
        });   
}

const authLogout = (state,action) => {
    return updateObject(
        state, {
            isGuest: false,
            user: null
        }
    )
}


const reducer = (state=initialState, action) => {

    switch (action.type) {
        case actionTypes.AUTH_START: return authStart(state,action);
        case actionTypes.AUTH_SUCCESS: return authSuccess(state,action);
        case actionTypes.AUTH_FAIL: return authFail(state,action);
        case actionTypes.AUTH_LOGOUT: return authLogout(state,action);
        default: return state;
    }
}

export default reducer;

