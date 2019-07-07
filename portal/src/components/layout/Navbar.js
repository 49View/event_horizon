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
      <div className="navbaruser-a">
        {userName}{" "}
        <a onClick={logout} href="#!">
          <i className="fas fa-sign-out-alt" /> Logout
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
  logout: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth
});

export default connect(
  mapStateToProps,
  { logout }
)(Navbar);
