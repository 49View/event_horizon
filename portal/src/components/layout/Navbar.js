import React, { Fragment } from "react";
import { Link } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { logout } from "../../actions/auth";

const Navbar = ({ userstate, logout }) => {
  let userName = ""; //auth.auth.user;
  //const user = JSON.parse(auth.auth.user);
  // console.log(userstate);

  if (userstate.isAuthenticated) {
    if (userstate.userdata && userstate.userdata.user.name) {
      userName = userstate.userdata.user.name;
    }
  }

  const authlinks = (
    <Fragment>
      <div className="user-navbar">
        {userName}
        <a onClick={logout} href="#!">
          <i className="fas fa-sign-out-alt" /> Logout
        </a>
      </div>
    </Fragment>
  );

  const nolinks = <Fragment />;

  return (
    <nav className="navbar bg-dark">
      <div className="navdiv">
        <div className="navdiv-title">
          <h1>
            <Link to="/">
              <span className="navdiv-titletext">
                <i className="fas fa-dice-d20" /> Event Horizon
              </span>
            </Link>
          </h1>
        </div>
        {userstate.isAuthenticated ? authlinks : nolinks}
      </div>
    </nav>
  );
};

Navbar.propTypes = {
  userstate: PropTypes.object,
  logout: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth
});

export default connect(
  mapStateToProps,
  { logout }
)(Navbar);
