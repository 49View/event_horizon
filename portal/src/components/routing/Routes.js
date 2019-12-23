import React, { Fragment } from "react";
import { Route, Switch } from "react-router-dom";
import Register from "../auth/Register";
import Login from "../auth/Login";
import DashboardUser from "../layout/DashboardUser";
import DashboardProject from "../layout/DashboardProject";
import NotFound from "../layout/NotFound";
import PrivateRoute from "../routing/PrivateRoute";
import AlertContainer from "../layout/AlertContainer";

const Routes = () => {
  return (
    <Fragment>
      <AlertContainer />
        <Switch>
          <Route exact path="/register" component={Register} />
          <Route exact path="/login" component={Login} />
          <PrivateRoute exact path="/dashboarduser" component={DashboardUser} />
          <Route exact path="/dashboardproject" component={DashboardProject} />
          <Route component={NotFound} />
        </Switch>
    </Fragment>
  );
};

export default Routes;
