import React, { Fragment } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { logout } from "../../actions/auth";

const Dashboard = ({ userstate, logout }) => {
  // console.log(userstate);

  if (!userstate.isAuthenticated) {
    return <Redirect to="/" />;
  }

  return <Fragment />;
};

Dashboard.propTypes = {
  // setAlert: PropTypes.func.isRequired,
  userstate: PropTypes.object,
  logout: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth
});

export default connect(
  mapStateToProps,
  { logout }
)(Dashboard);
