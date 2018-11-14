import axios from 'axios';
import * as actionTypes from './actionsTypes';

export const authStart = () => {
    return {
        type: actionTypes.AUTH_START
    };
}

export const authSuccess = (token) => {
    return {
        type: actionTypes.AUTH_SUCCESS,
        token: token,
    }
}

export const authFail = (error) => {
    return {
        type: actionTypes.AUTH_FAIL,
        error: error
    }
}

export const logout = () => {
    localStorage.removeItem('token');
    localStorage.removeItem('expirationDate');
    return {
        type: actionTypes.AUTH_LOGOUT
    }
}

export const checkAuthTimeout = (expirationTime) => {
    return dispatch => {
        setTimeout(() => {
            dispatch(logout());
        }, expirationTime*1000);
    }
}

export const auth = (email, password, project) => {
    return dispatch => {
        dispatch(authStart());
        const authData = {
            email: email,
            password: password,
            project: project
        };
        let url = '/getToken';
        axios.post(url, authData)
            .then(response => {
                console.log("AUTH SUCCESS");
                const expirationDate = new Date(new Date().getTime() + response.data.expires * 1000);

                localStorage.setItem('token', response.data.token);
                localStorage.setItem('expirationDate', expirationDate);
                dispatch(authSuccess(response.data.token));
                dispatch(checkAuthTimeout(response.data.expires));
            })
            .catch(err =>{
                dispatch(authFail(err.response.data.error));
                console.log("AUTH FAIL");
            })
    }
}

export const setAuthRedirectPath = (path) => {
    return {
        type: actionTypes.SET_AUTH_REDIRECT_PATH,
        path: path
    }
}

export const authCheckState = () => {
    return dispatch => {
        const token = localStorage.getItem('token');
        if (!token) {
            dispatch(logout());
        } else {
            const expirationDate = new Date(localStorage.getItem('expirationDate'));
            if (expirationDate<new Date()) {
                dispatch(logout());
            } else {
                dispatch(authSuccess(token));
                const expiresIn = (expirationDate.getTime() - new Date().getTime())/1000;
                dispatch(checkAuthTimeout(expiresIn));
            }
        }
    }
}