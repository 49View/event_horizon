import React, { Fragment, useState } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import Footer from "./Footer";
import { connect } from "react-redux";
import { setAlert } from "../../actions/alert";
import Moment from "react-moment";

const Dashboard = ({ isAuthenticated }) => {
  return (
    <Fragment>
      <h5 className="lead ">
        <i className="fas fa-columns" />
        &nbsp; Dashboard
      </h5>
    </Fragment>
  );
};

Dashboard.propTypes = {
  // setAlert: PropTypes.func.isRequired,
  isAuthenticated: PropTypes.bool
};

const mapStateToProps = state => ({
  isAuthenticated: state.auth.isAuthenticated
});

export default connect(
  mapStateToProps,
  {}
)(Dashboard);
