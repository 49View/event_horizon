import React, { Fragment, useEffect } from "react";
import { BrowserRouter as Router, Route, Switch } from "react-router-dom";
import Landing from "./components/layout/Landing";
import Routes from "./components/routing/Routes";

// Redux
import { Provider } from "react-redux";
import store from "./store";

import { loadUser } from "./actions/auth";
import setAuthToken from "./utils/setAuthToken";
import axios from "axios";

import "./App.css";

if (!process.env.NODE_ENV || process.env.NODE_ENV === "development") {
  // dev code
} else {
  axios.defaults.baseURL = "https://api.ateventhorizon.com";
  // production code
}

if (localStorage.token) {
  setAuthToken(localStorage.token);
}

const App = () => {
  useEffect(() => {
    store.dispatch(loadUser());
  }, []);

  // window.SocketClientHandler.send({
  //   msg: "loadAsset",
  //   name: "shelf",
  //   project: "carillo"
  // });

  return (
    <Provider store={store}>
      <Router>
        <Fragment>
          <Switch>
            <Route exact path="/" component={Landing} />
            <Route component={Routes} />
          </Switch>
        </Fragment>
      </Router>
    </Provider>
  );
};

export default App;
