import React, { Component } from 'react';
import { withRouter } from 'react-router-dom';
import { connect } from 'react-redux';
import * as actions from './store/actions/index';

import Render from './components/render/Render';
import classes from './App.css';

import socketClientHandlerFactory from './extra/socketClientHandlerFactory';
window.SocketClientHandler = socketClientHandlerFactory();

class App extends Component {

  componentDidMount() {
    this.props.onCheckCurrentUser();
    this.props.onLoadWasm("editor");
  }


  render() {

    let render = null;
    if (this.props.isWasmLoaded && (this.props.isAuthenticated || this.props.isGuest)) {
        render = <Render/>
    }

    return (
      <div className={classes.App}>
        {render}
      </div>
    );
  }
}

const mapStateToProps = state => {
  return {
    isAuthenticated: state.auth.user !== null,
    isGuest: state.auth.isGuest,
    isWasmLoaded: state.wasm.loaded,
    wasmError: state.wasm.error
}
}

const mapDispatchToProps = dispatch => {
  return {
    onCheckCurrentUser: () => dispatch(actions.checkCurrentUser()),
    onLoadWasm: (project) => dispatch(actions.loadWasm(project))
  }
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(App));
