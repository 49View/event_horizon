import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

import axios from 'axios';

import sio from 'socket.io-client';

class App extends Component {

  state = {
    onLoggingIn: false,
    onConnectingSocket: false,
    logged: false,
    socket: null,
  }

  onRequiredConnectSocket = (event) => {
    if (this.state.onConnectingSocket) {
      return;
    }

    this.setState({ onConnectingSocket: true});
    const socket = sio('https://localhost:3000', );
    socket.on('connect', () => {
      console.log("Connected to socket");
      this.setState({socket: socket});
      socket.emit('message', { msg: "ping"});
      socket.emit('message', { msg: "cloudStorageFileUpdate-elaborate/geom/plant_spider_big.fbx"});
      socket.emit('message', { msg : 'floorplan2dto3d', property_listing : 'https://www.rightmove.co.uk/property-for-sale/property-55483515.html'});

      socket.on('message', (message) => {
        console.log("Received message: ", message);
      })
      socket.on('disconnect', () => {
        console.log("Disconnect");
        this.setState({socket: null});
      })
  
    });
    this.setState({ onConnectingSocket: false});
  }

  onRequiredDisconnectSocket = () => {
    this.state.socket.disconnect();
  }

  onRequiredLogin = async () => {
    if (this.state.onLoggingIn) {
      return;
    }
    this.setState({ onLoggingIn: true});
    console.log("LOGIN");
    try {

      const result = await axios.post(
        "https://localhost:3000/getToken",
        {
          "project": "49view",
          "email": "guest@49view.com",
          "password": "guest"
        },
        { withCredentials: true}
      );
      console.log("RESULT: ",result);
      this.setState({logged: true});
    } catch (ex) {
      console.log("ERROR DURING LOGIN", ex);
    }
    this.setState({ onLoggingIn: false});
  }

  onRequiredLogout = async () => {
    console.log("LOGOUT");
    try {
      const result = await axios.get(
        "https://localhost:3000/cleanToken",
        { withCredentials: true}
      );
      console.log("RESULT: ",result);
      this.setState({logged: false});
    } catch (ex) {
      console.log("ERROR DURING LOGIN", ex);
    }
  }

  render() {
    return (
      <div className="App">
        <header className="App-header">
          <button disabled={this.state.logged && !this.state.onLoggingIn} onClick={this.onRequiredLogin}>LOGIN</button>
          <button disabled={!this.state.logged} onClick={this.onRequiredLogout}>LOGOUT</button>
          <button disabled={this.state.socket!==null && !this.state.onConnectingSocket} onClick={this.onRequiredConnectSocket}>CONNECT SOCKET</button>
          <button disabled={this.state.socket===null} onClick={this.onRequiredDisconnectSocket}>DISCONNECT SOCKET</button>
        </header>
      </div>
    );
  }
}

export default App;
