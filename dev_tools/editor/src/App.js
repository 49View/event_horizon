import React, { Component } from 'react';
import { Route, Switch, withRouter, Redirect } from 'react-router-dom';
import { connect } from 'react-redux';
import * as actions from './store/actions/index';

import Render from './components/render/Render';
import classes from './App.css';

import socketClientHandlerFactory from './extra/socketClientHandlerFactory';

window.SocketClientHandler = socketClientHandlerFactory();

class App extends Component {

  onClickHandler = () => {
    alert("CLICK");
  }

  componentDidMount() {
    this.props.onCheckCurrentUser();
  }


  render() {
    return (
      <div className={classes.App}>
        <Render/>
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
    onCheckCurrentUser: () => dispatch(actions.checkCurrentUser())
  }
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(App));
