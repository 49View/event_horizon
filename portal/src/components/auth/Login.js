import React, { Fragment, useState } from "react";
import { Link, Redirect } from "react-router-dom";
import { connect } from "react-redux";
import PropTypes from "prop-types";
import { login } from "../../actions/auth";

const Login = ({ login, isAuthenticated, user }) => {
  const [formData, setFromData] = useState({
    email: "",
    password: "",
    project: ""
  });

  const { email, password, project } = formData;
  const onChange = e =>
    setFromData({
      ...formData,
      [e.target.name]: e.target.value
    });
  const onSubmit = e => {
    e.preventDefault();
    login(email, password, project);
  };

  // Redirect if logged in
  if (isAuthenticated) {
    console.log("Redirecting to dashboard...");
    return <Redirect to="/dashboarduser" />;
  }

  return (
    <Fragment>
      <section className="container">
        <h1 className="large text-primary">
          <br />
          Login
        </h1>
        <p className="lead">
          <i className="fas fa-user" /> Log-in Into Your Account
        </p>
        <form className="form" onSubmit={e => onSubmit(e)}>
          <div className="form-group">
            <input
              type="email"
              placeholder="Email Address"
              autoComplete="email"
              name="email"
              value={email}
              onChange={e => onChange(e)}
            />
          </div>
          <div className="form-group">
            <input
              type="password"
              placeholder="Password"
              name="password"
              autoComplete="current-password"
              value={password}
              onChange={e => onChange(e)}
              minLength="6"
            />
          </div>
          <div className="form-group">
            <input
              type="text"
              placeholder="Project"
              name="project"
              value={project}
              onChange={e => onChange(e)}
            />
          </div>
          <input type="submit" className="btn btn-primary" value="Login" />
        </form>
        <p className="my-3">
          Don't have an account? <Link to="/register">Sign Up</Link>
        </p>
      </section>
    </Fragment>
  );
};

Login.propTypes = {
  login: PropTypes.func.isRequired,
  isAuthenticated: PropTypes.bool,
  user: PropTypes.object
};

const mapStateToProps = state => ({
  isAuthenticated: state.auth.isAuthenticated,
  user: state.auth.user
});

export default connect(
  mapStateToProps,
  { login }
)(Login);
