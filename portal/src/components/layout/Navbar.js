import React from "react";
import { Link } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const Navbar = auth => {
  const proj = auth.auth.project;
  const userName = ""; //auth.auth.user;
  //const user = JSON.parse(auth.auth.user);
  //console.log(auth.auth.user);

  return (
    <nav className="navbar bg-dark">
      <div className="navdiv">
        <div className="navdiv-title">
          <h1>
            <Link to="/">
              <i className="fas fa-code" /> Event Horizon
            </Link>
          </h1>
        </div>
        {/* <div className="user-navbar">Project: {proj}</div>
        <div className="user-navbar">Name: {userName}</div> */}
      </div>
    </nav>
  );
};

Navbar.propTypes = {
  auth: PropTypes.object
};

const mapStateToProps = state => ({
  auth: state.auth
});

export default connect(
  mapStateToProps,
  {}
)(Navbar);
