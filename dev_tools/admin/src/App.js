import React, { Component } from 'react';
import { connect } from 'react-redux';
import { Route, Switch, withRouter, Redirect } from 'react-router-dom';
import Dialog from '@material-ui/core/Dialog';
import DialogContent from '@material-ui/core/DialogContent';
import DialogContentText from '@material-ui/core/DialogContentText';
import * as actions from './store/actions/index';
import Layout from './components/Layout/Layout';
import Dashboard from './components/Dashboard/Dashboard';
import Login from './components/Login/Login';
import Logout from './components/Logout/Logout';
import Aux from './hoc/Auxiliary/Auxiliary';
import CircularProgress from '@material-ui/core/CircularProgress';
import classes from './App.module.css';


class App extends Component {

  componentDidMount() {
    this.props.onCheckCurrentUser();
  }

  render() {
    let routes = (
      <Switch>
        <Route path="/dashboard" component={Dashboard}/>
        <Route path="/login" component={Login}/>
        <Route path="/logout" component={Logout}/>
        <Route path="/" exact component={null}/>
        <Redirect to="/"/>
      </Switch>
    );

    // if (this.props.loggedUser!==null) {
    //   routes = (
    //     <Switch>
    //       <Route path="/dashboard" component={Dashboard}/>
    //       <Route path="/logout" component={Logout}/>
    //       <Route path="/" exact component={null}/>
    //       <Redirect to="/dashboard"/>
    //     </Switch>
    //   );
    // } else if (this.props.isGuest) {
    //   routes = (
    //     <Switch>
    //       <Route path="/login" component={Login}/>
    //       <Route path="/" exact component={null}/>
    //       {/* <Redirect to="/login"/> */}
    //     </Switch>
    //   );
    // }

    return (
      <Aux>
        <Dialog
          disableBackdropClick
          disableEscapeKeyDown
          maxWidth="xs"                
          open={this.props.authLoading}
        >
          <DialogContent>
              <DialogContentText className={classes.WaitText}>
                  Attendere
              </DialogContentText>
              <CircularProgress color="primary" className={classes.Progress}></CircularProgress>
          </DialogContent>
        </Dialog>
        <Layout isAuthenticated={this.props.loggedUser!==null} user={this.props.loggedUser} project={this.props.loggedProject}>
          {routes}
        </Layout>
      </Aux>
    );
  }
}

const mapStateToProps = state => {
  return {
      loggedUser: state.auth.user,
      loggedProject: state.auth.project,
      authLoading: state.auth.loading,
  }
}

const mapDispatchToProps = dispatch => {
  return {
    onCheckCurrentUser: () => dispatch(actions.checkCurrentUser()),
  }
}

export default withRouter(connect(mapStateToProps,mapDispatchToProps)(App));

