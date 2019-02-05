import axios from '../../axios/backend';
import * as actionTypes from './actionsTypes';


export const authStart = () => {
    return {
        type: actionTypes.AUTH_START
    };
}

export const authSuccess = (userInfo) => {
    let isGuest=false;
    let user=null;
    if (userInfo.user.guest===true) {
        isGuest=true;
    } else {
        isGuest=false;
        user=userInfo.user;
    }
    let msToExpiration = userInfo.expires*1000-new Date().getTime();
    console.log("Token expires in: "+msToExpiration+" ms");
    //dispatch(checkAuthTimeout(userInfo.expires));
    return {
        type: actionTypes.AUTH_SUCCESS,
        isGuest: isGuest,
        user: user
    }
}

export const authFail = (error) => {
    return {
        type: actionTypes.AUTH_FAIL,
        error: error
    }
}

export const logout = () => {
    return {
        type: actionTypes.AUTH_LOGOUT
    }
}

export const checkAuthTimeout = (expirationDate) => {
    return dispatch => {

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
                console.log(response);
                const expirationDate = new Date(response.data.expires);
                console.log("EXPIRATION DATE: "+expirationDate);
                dispatch(authSuccess(response.data));
                //dispatch(checkAuthTimeout(response.data.expires));
            })
            .catch(err =>{
                dispatch(authFail(err.response.data.error));
                console.log("AUTH FAIL");
            })
    }
}

export const checkCurrentUser = () => {
    return dispatch => {
        console.log("CHECK CURRENT USER");
        let url = '/user';
        axios.get(url)
            .then(response => {
                console.log("CHECK CURRENT USER SUCCESS");
                dispatch(authSuccess(response.data));
            })
            .catch(err =>{
                console.log("CHECK CURRENT USER FAILED");
                dispatch(authFail("Error getting user information"));
            })
    }
}