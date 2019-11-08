import React, { Fragment, useEffect } from "react";
import { HashRouter as Router, Route, Switch } from "react-router-dom";
import Landing from "./components/layout/Landing";
import Navbar from "./components/layout/Navbar";
import Routes from "./components/routing/Routes";

import { initHostEnv, getApiHostName } from "./HostEnv";

import { DBConfig } from "./DBConfig";
import { initDB } from "react-indexed-db";

// Redux
import { Provider } from "react-redux";
import store from "./store";

import { loadUser } from "./actions/auth";

import "./App.css";
import WasmCanvas from "./components/layout/WasmCanvas";

initHostEnv();
initDB(DBConfig);

const App = () => {
  useEffect(() => {
    store.dispatch(loadUser());
  });

  const wasmArgumentList = [getApiHostName()];

  return (
    <Provider store={store}>
      <Router>
        <WasmCanvas
          wasmName="editor"
          argumentList={wasmArgumentList}
        ></WasmCanvas>
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
