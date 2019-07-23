import React from "react";
import { Link, Redirect } from "react-router-dom";
import { connect } from "react-redux";
import PropTypes from "prop-types";

const Landing = auth => {
  if (auth.auth && auth.auth.isAuthenticated === true) {
    return <Redirect to="/dashboarduser" />;
  }

  return (
    <section className="landing">
      <div className="dark-overlay">
        <div className="landing-inner">
          <h1 className="x-large">Event Horizon</h1>
          <p className="lead">Bring it on</p>
          <div className="buttons">
            <Link to="./register" className="btn btn-primary">
              Sign Up
            </Link>
            <Link to="./login" className="btn btn-light">
              Login
            </Link>
          </div>
        </div>
      </div>
    </section>
  );
};

Landing.propTypes = {
  auth: PropTypes.object
};

const mapStateToProps = state => ({
  auth: state.auth
});

export default connect(
  mapStateToProps,
  {}
)(Landing);
