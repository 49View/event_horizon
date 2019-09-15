import React, { Fragment } from "react";
import { Route, Switch } from "react-router-dom";
import Register from "../auth/Register";
import Login from "../auth/Login";
import DashboardUser from "../layout/DashboardUser";
import DashboardProject from "../layout/DashboardProject";
import NotFound from "../layout/NotFound";
import PrivateRoute from "../routing/PrivateRoute";
import Alert from "../../components/layout/Alert";
import { transitions, positions, Provider as AlertProvider } from "react-alert";
import AlertTemplate from "react-alert-template-basic";

// optional cofiguration
const alertOptions = {
  // you can also just use 'bottom center'
  position: positions.MIDDLE,
  timeout: 5000,
  offset: "30px",
  // you can also just use 'scale'
  transition: transitions.SCALE
};

const Routes = () => {
  return (
    <Fragment>
      <Alert />
      <AlertProvider template={AlertTemplate} {...alertOptions}>
        <Switch>
          <Route exact path="/register" component={Register} />
          <Route exact path="/login" component={Login} />
          <PrivateRoute exact path="/dashboarduser" component={DashboardUser} />
          <Route exact path="/dashboardproject" component={DashboardProject} />
          <Route component={NotFound} />
        </Switch>
      </AlertProvider>
    </Fragment>
  );
};

export default Routes;
