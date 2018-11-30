import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

import sio from 'socket.io-client';

class App extends Component {

  state = {
    socket: null
  }

  componentDidMount() {
    const socket = sio('https://localhost:3000', );
    socket.on('connect', () => {
      console.log("Connected to socket");
      socket.emit('message', { msg: "ping"});
      socket.emit('message', { msg: "cloudStorageFileUpdate-elaborate/geom/plant_spider_big.fbx"});
      socket.emit('message', { msg : 'floorplan2dto3d', property_listing : 'https://www.rightmove.co.uk/property-for-sale/property-55483515.html'});
    });
    socket.on('message', (message) => {
      console.log("Received message: ", message);
    })
    socket.on('disconnect', () => {
      console.log("Disconnect");
    })
  }


  render() {
    return (
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <p>
            Edit <code>src/App.js</code> and save to reload.
          </p>
          <a
            className="App-link"
            href="https://reactjs.org"
            target="_blank"
            rel="noopener noreferrer"
          >
            Learn React
          </a>
        </header>
      </div>
    );
  }
}

export default App;
