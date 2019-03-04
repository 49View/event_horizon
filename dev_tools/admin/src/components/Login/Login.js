
import React, { Component } from 'react'
import { connect } from 'react-redux';
import ReactDOM from 'react-dom';
import * as actions from '../../store/actions/index';
import {Redirect, NavLink} from 'react-router-dom';
import Grid from '@material-ui/core/Grid';
import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import FormControl from '@material-ui/core/FormControl';
import Select from '@material-ui/core/Select';
import OutlinedInput from '@material-ui/core/OutlinedInput';
import InputLabel from '@material-ui/core/InputLabel';
import classes from './Login.module.css';

class Login extends Component {

    state={
        username: '',
        password: '',
        project: '',
        projectLabelWidth: 0
    }

    componentDidMount() {
        this.setState({
            projectLabelWidth: ReactDOM.findDOMNode(this.InputProjectLabelRef).offsetWidth,
          });        
    }

    onLoginHandler = () => {
        this.props.onLogin(this.state.username,this.state.password,this.state.project);
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
                                SIGNIN USER
                            </Typography>
                            {error}
                            <br/>
                            <FormControl variant="outlined" className={classes.formControl} 
                                fullWidth 
                                required 
                                placeholder="project"
                                margin="normal"
                            >
                                <InputLabel
                                    ref={ref => {
                                        this.InputProjectLabelRef = ref;
                                    }}
                                    htmlFor="outlined-project-field"
                                >
                                    project
                                </InputLabel>
                                <Select
                                    native
                                    fullWidth
                                    required={true}
                                    value={this.state.project}
                                    onChange={this.onChangeFieldHandler('project')}
                                    input={
                                        <OutlinedInput
                                            name="project"
                                            labelWidth={this.state.projectLabelWidth}
                                            id="outlined-project-field"
                                        />
                                    }
                                >
                                    <option value="" />
                                    <option value="49View">49View</option>
                                    <option value="Sumix">Sumix</option>
                                </Select>
                            </FormControl>
                            <TextField
                                required={true}
                                autoComplete="false"
                                label="e-mail"
                                placeholder="e-mail"
                                helperText="Insert your e-mail address"
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
                                helperText="Insert password"
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
      onLogin: (user,password,project) => dispatch(actions.login(user,password,project)),
    }
}
  
export default connect(mapStateToProps,mapDispatchToProps)(Login);
