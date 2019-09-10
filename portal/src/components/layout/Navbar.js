import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { logoffFromProject } from "../../actions/auth";
import Spinner from "react-bootstrap/Spinner";

const Navbar = ({ userstate, logoffFromProject, loading, loading2 }) => {
  let userName = "";

  if (userstate.isAuthenticated) {
    if (userstate.userdata && userstate.userdata.user.name) {
      userName = userstate.userdata.user.name;
    }
  }

  const authlinks = (
    <Fragment>
      <div className="navbaruser-a">
        <a onClick={logoffFromProject} href="/#/dashboarduser">
          <i className="fas fa-user" /> {userName}
        </a>
      </div>
    </Fragment>
  );

  const nolinks = <Fragment />;
  const title = userstate.userdata ? userstate.userdata.project : "";

  return (
    <div className="navbarGrid">
      <div className="navbarlogo-a">
        {loading || loading2 ? (
          <Spinner animation="border" variant="warning" />
        ) : (
          <img src="/ehlogo.svg" alt="" />
        )}
      </div>
      <div className="navbareh-a navdiv-titletext">
        {" "}
        <span className="colorLogo1">E</span>
        <span>vent</span> <span className="colorLogo2">H</span>
        <span>orizon</span>
      </div>
      <div className="navbartitle-a">{title}</div>
      {userstate.isAuthenticated ? authlinks : nolinks}
    </div>
  );
};

Navbar.propTypes = {
  userstate: PropTypes.object,
  loading: PropTypes.bool,
  loading2: PropTypes.bool,
  logoffFromProject: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  loading: state.auth.loading,
  loading2: state.entities.loading
});

export default connect(
  mapStateToProps,
  { logoffFromProject }
)(Navbar);
