import React, { Component } from 'react';

import Render from './Render/Render';
import classes from './App.css';
import socketClientHandlerFactory from './Extra/socketClientHandlerFactory';
import wasmLoader from './Extra/wasmLoader';

window.SocketClientHandler = socketClientHandlerFactory();
window.wasmLoader = new wasmLoader();

class App extends Component {

  render() {
    return (
      <div className={classes.App}>
        <Render />
      </div>
    );
  }

}

export default App;
