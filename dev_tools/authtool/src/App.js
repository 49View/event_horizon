import React, { Component } from 'react';
import { Route, Switch, withRouter, Redirect } from 'react-router-dom';
import { connect } from 'react-redux';
import * as actions from './store/actions/index';

import classes from './App.css';

import Stars from './components/UI/Stars/Stars';
import Login from './components/Login/Login';

class App extends Component {

  onClickHandler = () => {
    alert("CLICK");
  }

  render() {
    return (
      <div className={classes.App}>
        <Stars/>
        <Login/>
      </div>
    );
  }
}

const mapStateToProps = state => {
  return {
    isAuthenticated: state.auth.token != null
  }
}

const mapDispatchToProps = dispatch => {
  return {
    onTryAutoSignin: () => dispatch(actions.authCheckState())
  }
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(App));
