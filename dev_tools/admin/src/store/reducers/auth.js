import * as actionTypes from '../actions/actionsTypes';
import { updateObject} from '../utility';

const initialState = {
    error: null,
    loading: false,
    isGuest: false,
    user: null
}

const refreshStart = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: true
        });
}

const refreshComplete = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: false,
            isGuest: false,
            user: action.user            
        });
}

const refreshFail = (state,action) => {
    return updateObject(
        state, {
            error: action.error,
            loading: false
        });   
}

const loginStart = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: true
        });
}

const loginComplete = (state,action) => {
    return updateObject(
        state, {
            error: null,
            loading: false,
            isGuest: action.isGuest,
            user: action.user            
        });
}

const loginFail = (state,action) => {
    return updateObject(
        state, {
            error: action.error,
            loading: false
        });   
}

const logoutStart = (state,action) => {
    return updateObject(
        state, {
            loading: true
        }
    )
}

const logoutComplete = (state,action) => {
    return updateObject(
        state, {
            loading: false,
            user: null,
            isGuest: true
        }
    )
}

const logoutFail = (state,action) => {
    return updateObject(
        state, {
            error: action.error,
            loading: false
        });   
}

const reducer = (state=initialState, action) => {

    switch (action.type) {
        case actionTypes.AUTH_LOGIN_START: return loginStart(state,action);
        case actionTypes.AUTH_LOGIN_COMPLETE: return loginComplete(state,action);
        case actionTypes.AUTH_LOGIN_FAIL: return loginFail(state,action);
        case actionTypes.AUTH_LOGOUT_START: return logoutStart(state,action);
        case actionTypes.AUTH_LOGOUT_COMPLETE: return logoutComplete(state,action);
        case actionTypes.AUTH_LOGOUT_FAIL: return logoutFail(state,action);
        case actionTypes.AUTH_REFRESH_START: return refreshStart(state,action);
        case actionTypes.AUTH_REFRESH_COMPLETE: return refreshComplete(state,action);
        case actionTypes.AUTH_REFRESH_FAIL: return refreshFail(state,action);
        default: return state;
    }
}

export default reducer;

