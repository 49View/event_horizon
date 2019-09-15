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
      <div className="landing-inner">
        <h1 className="logofont x-large blooming">
          <span className="colorLogo1">T</span>
          <span className="colorLogo2">h</span>
          <span>e</span>
          <span className="colorLogo2"> </span>
          <span className="colorLogo1">C</span>
          <span className="colorLogo2">r</span>
          <span>e</span>
          <span className="colorLogo2">a</span>
          <span className="colorLogo1">t</span>
          <span>o</span>
          <span className="colorLogo1">r</span>
          <span className="colorLogo2">s</span>
          <br></br>
          <span className="colorLogo1">P</span>
          <span className="colorLogo2">l</span>
          <span>a</span>
          <span className="colorLogo2">y</span>
          <span className="colorLogo1">g</span>
          <span className="colorLogo2">r</span>
          <span>o</span>
          <span className="colorLogo2">u</span>
          <span className="colorLogo1">n</span>
          <span>d</span>
        </h1>
        <div className="my-2"></div>
        <div className="buttons">
          <Link to="./register" className="btn btn-primary my-2">
            Sign Up
          </Link>{" "}
          <Link to="./login" className="btn btn-light my-2">
            Login
          </Link>
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
