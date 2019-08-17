import React, { Fragment } from "react";
import { Link } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { logoffFromProject } from "../../actions/auth";

const Navbar = ({ userstate, logoffFromProject }) => {
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

  return (
    <div className="navbarGrid">
      <div className="navbareh-a">
        <Link to="/">
          <span className="navdiv-titletext">
            <i className="fas fa-dice-d20" /> Event Horizon
          </span>
        </Link>
      </div>
      {userstate.isAuthenticated ? authlinks : nolinks}
    </div>
  );
};

Navbar.propTypes = {
  userstate: PropTypes.object,
  logoffFromProject: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth
});

export default connect(
  mapStateToProps,
  { logoffFromProject }
)(Navbar);
