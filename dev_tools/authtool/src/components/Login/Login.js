import React, {Component} from 'react';
import classes from './Login.css';

import Button from '../UI/Button/Button';

class Login extends Component {
    render() {
        return (
            <div className={classes.Login}>
                <div className={classes.Title}>EVENT HORIZON</div>
                <div className={classes.Form}></div>
                <div className={classes.Buttons}>
                    <Button btnType="Success">LOGIN</Button>
                    <Button btnType="Success">REGISTER</Button>
                </div>
            </div>
        );
    }
}

export default Login;