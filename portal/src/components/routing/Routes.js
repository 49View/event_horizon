import React, { Fragment } from "react";
import { Route, Switch } from "react-router-dom";
import Register from "../auth/Register";
import Login from "../auth/Login";
import Alert from "../layout/Alert";
import Dashboard from "../layout/Dashboard";
import NotFound from "../layout/NotFound";
import PrivateRoute from "../routing/PrivateRoute";
import Navbar from "../../components/layout/Navbar";

const Routes = () => {
  return (
    <Fragment>
      <div className="mainDesktopPageLayout">
        <Navbar />
        <Alert />
        <Switch>
          <Route exact path="/register" component={Register} />
          <Route exact path="/login" component={Login} />
          <PrivateRoute exact path="/dashboard" component={Dashboard} />
          <Route component={NotFound} />
        </Switch>
      </div>
    </Fragment>
  );
};

export default Routes;
