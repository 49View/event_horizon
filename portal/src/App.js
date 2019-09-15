import React, { Fragment, useEffect } from "react";
import { HashRouter as Router, Route, Switch } from "react-router-dom";
import Landing from "./components/layout/Landing";
import Navbar from "./components/layout/Navbar";
import Routes from "./components/routing/Routes";

import { DBConfig } from "./DBConfig";
import { initDB } from "react-indexed-db";

// Redux
import { Provider } from "react-redux";
import store from "./store";

import { loadUser } from "./actions/auth";
import axios from "axios";

import "./App.css";
import WasmCanvas from "./components/layout/WasmCanvas";

if (process.env.NODE_ENV && process.env.NODE_ENV === "development") {
  // dev code
  axios.defaults.baseURL = "https://localhost:3000";
} else {
  axios.defaults.baseURL = "https://api.ateventhorizon.com";
  // production code
}
axios.defaults.withCredentials = true;

// console.log("REACT_APP_USE_API:" + process.env.REACT_APP_USE_API);
if (
  process.env.REACT_APP_USE_API &&
  process.env.REACT_APP_USE_API === "production"
) {
  axios.defaults.baseURL = "https://api.ateventhorizon.com";
}

initDB(DBConfig);

const App = () => {
  useEffect(() => {
    store.dispatch(loadUser());
  });

  return (
    <Provider store={store}>
      <Router>
        <WasmCanvas></WasmCanvas>
        <Navbar />
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
