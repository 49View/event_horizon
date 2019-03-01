
import React, { Component } from 'react'
import { connect } from 'react-redux';
import * as actions from '../../store/actions/index';
import {Redirect, NavLink} from 'react-router-dom';
import Grid from '@material-ui/core/Grid';
import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import classes from './Login.module.css';

class Login extends Component {

    state={
        username: '',
        password: ''
    }

    onLoginHandler = () => {
        this.props.onLogin(this.state.username,this.state.password);
    }

    onChangeFieldHandler = name => event => {
        this.setState({ [name]: event.target.value });
    };

    render() {

        let error=null;
        if (this.props.loginError) {
            error = (
                <Typography variant="subtitle2">
                    {this.props.loginError}
                </Typography>
            );
        }

        let result = <Redirect to="/dashboard"></Redirect>
        if (!this.props.isAuthenticated) {
            result = (
                <form className={classes.Form} noValidate autoComplete="off">
                    <Grid container className={classes.Container}>
                        <Grid item xs={12} sm={10}  md={8} lg={6} className={classes.Item}>
                            <Typography variant="h5">
                                ACCESSO UTENTI
                            </Typography>
                            {error}
                            <TextField
                                required={true}
                                autoComplete="false"
                                label="e-mail"
                                placeholder="e-mail"
                                helperText="Inserire l'indirizzo e-mail del proprio account"
                                fullWidth
                                value={this.state.username}
                                onChange={this.onChangeFieldHandler('username')}
                                margin="normal"
                                variant="outlined"
                                InputLabelProps={{
                                    shrink: true,
                                }}
                            />
                            <TextField
                                required={true}
                                autoComplete="false"
                                type="password"                            
                                label="password"
                                placeholder="password"
                                helperText="Inserire la password"
                                fullWidth
                                value={this.state.password}
                                onChange={this.onChangeFieldHandler('password')}
                                margin="normal"
                                variant="outlined"
                                InputLabelProps={{
                                    shrink: true,
                                }}
                            />
                            <Grid container justify="flex-end" alignItems="center" direction="row" spacing={16}>
                                <Grid item>
                                    <Button variant="outlined" color="primary" onClick={this.onLoginHandler}>
                                        Accedi
                                    </Button>
                                </Grid>
                                <Grid item>
                                    <Button variant="outlined" color="primary" component={NavLink} to="/">
                                        Annulla
                                    </Button>                        
                                </Grid>
                            </Grid>
                        </Grid>
                    </Grid> 
                </form>
            );
        }
        return result;
    }
}

 
const mapStateToProps = state => {
    return {
        loginError: state.auth.error,
        isAuthenticated: state.auth.user!==null
    }
} 
const mapDispatchToProps = dispatch => {
    return {
      onLogin: (user,password) => dispatch(actions.login(user,password)),
    }
}
  
export default connect(mapStateToProps,mapDispatchToProps)(Login);
