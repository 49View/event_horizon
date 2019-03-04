import axios from '../../axios/backend';
import * as actionTypes from './actionsTypes';

let refreshTimer = null;

const refresh = (dispatch) => {

    console.log("REFRESH");
    dispatch(refreshStart());
    let url = '/refreshToken?d='+new Date().getTime();
    axios.get(url)
        .then(response => {
            console.log("REFRESH SUCCESS");
            console.log(response);
            dispatch(refreshComplete(response.data));
            dispatch(setRefreshTimer(response.data.expires));
        })
        .catch(err =>{
            let error=err.response.statusText;
            dispatch(refreshFail(error));
            console.log("REFRESH FAIL", err.response);
        });
}

const refreshStart = () => {
    return {
        type: actionTypes.AUTH_REFRESH_START
    };
}

const refreshComplete = (userInfo) => {
    let user=userInfo.user;
    return {
        type: actionTypes.AUTH_REFRESH_COMPLETE,
        user: user
    }
}

const refreshFail = (error) => {
    return {
        type: actionTypes.AUTH_REFRESH_FAIL,
        error: error
    }
}

export const login = (email, password, project) => {
    return dispatch => {
        console.log("LOGIN");
        dispatch(loginStart());
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
                dispatch(loginComplete(response.data));
                dispatch(setRefreshTimer(response.data.expires));
            })
            .catch(err =>{
                let error=err.response.statusText;
                if (err.response.status===401) {
                    error="email/password non corretti";
                }
                dispatch(loginFail(error));
                console.log("AUTH FAIL", err.response);
            })
    }
}

const loginStart = () => {
    return {
        type: actionTypes.AUTH_LOGIN_START
    };
}

const loginComplete = (userInfo) => {
    let user=null;
    let project=null;
    console.log("USERINFO: "+userInfo);
    if (userInfo.user!==null) {
        user=userInfo.user;
        project=userInfo.project;
    }
    return {
        type: actionTypes.AUTH_LOGIN_COMPLETE,
        user: user,
        project: project
    }
}

const loginFail = (error) => {
    return {
        type: actionTypes.AUTH_LOGIN_FAIL,
        error: error
    }
}

export const logout = () => {
    return dispatch => {
        console.log("LOGOUT");
        dispatch(logoutStart());
        let url = '/cleanToken?d='+new Date().getTime();
        axios.get(url)
            .then(response => {
                console.log("LOGOUT SUCCESS");
                dispatch(logoutComplete());
                resetRefreshTimer();
            })
            .catch(err => {
                console.log("LOGOUT ERROR");
                dispatch(logoutFail("Network error"));
            });
    }
}

const logoutStart = () => {
    return {
        type: actionTypes.AUTH_LOGOUT_START
    }
}

const logoutComplete = () => {
    return {
        type: actionTypes.AUTH_LOGOUT_COMPLETE
    }
}

const logoutFail = (error) => {
    return {
        type: actionTypes.AUTH_LOGOUT_FAIL,
        error: error
    }
}

const resetRefreshTimer = () => {
    console.log("RESET REFRESH TIMER");
    if  (refreshTimer!==null) {
        clearTimeout(refreshTimer);
        refreshTimer=null;
    }

}

const setRefreshTimer = (expirationDate) => {
    return dispatch => {
        resetRefreshTimer();
        let currentDate = Math.floor(new Date().getTime()/1000);
        let sToExpiration = expirationDate-(5*60)-currentDate;
        console.log("Token expires in: "+(expirationDate-currentDate)+" s");
        console.log("Token refresh in: "+sToExpiration+" s");
        if (sToExpiration>0) {
            refreshTimer=setTimeout(() => {
                resetRefreshTimer();
                refresh(dispatch);
            }, sToExpiration*1000);
        }
    }
}

export const checkCurrentUser = () => {
    return dispatch => {
        console.log("CHECK CURRENT USER");
        let url = '/user';
        axios.get(url)
            .then(response => {
                console.log("USER IS LOGGED IN", response.data);
                dispatch(loginComplete(response.data));
                console.log("REGISTERED USER");
                dispatch(setRefreshTimer(response.data.expires));
            })
            .catch(err =>{
                console.log("USER ISN'T LOGGED IN");
                dispatch(logoutComplete());
            })
    }
}