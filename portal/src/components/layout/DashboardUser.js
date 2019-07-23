import React, { Fragment, useState } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "./Spinner";
import { createProject, setCurrentProject } from "../../actions/auth";

const DashboardUser = ({
  userstate,
  loading,
  createProject,
  setCurrentProject
}) => {
  const [newProjectformData, setNewProjectformData] = useState({
    projectNew: ""
  });

  if (
    userstate.isAuthenticated &&
    userstate.userdata &&
    userstate.userdata.project !== ""
  ) {
    return <Redirect to="/dashboard/geoms" />;
  }

  //   const onRequestProject = e => {
  //     e.preventDefault();
  //   };

  const onChange = e => {
    setNewProjectformData({
      ...newProjectformData,
      [e.target.name]: e.target.value
    });
  };

  const onCreateProject = e => {
    e.preventDefault();
    console.log("urca");
    createProject(newProjectformData.projectNew);
  };

  const onLoginToProject = e => {
    e.preventDefault();
    setCurrentProject(e.target.value);
  };

  console.log("DashboardUser render");

  if (loading || userstate.userdata === null) return <Spinner />;

  let userProjects;
  if (userstate.userdata.projects !== null) {
    userProjects = (
      <Fragment>
        <div className="yourproject">Your Projects:</div>
        <div className="project-login">
          {userstate.userdata.projects.map(projectObject => (
            <input
              type="button"
              className="btn btn-primary"
              value={projectObject.project}
              onClick={e => onLoginToProject(e)}
            />
          ))}
        </div>
      </Fragment>
    );
  } else {
    userProjects = (
      <Fragment>
        <div className="yourproject lead">
          <i className="fas fa-chess-queen" /> Your Projects
        </div>
        <span className="normal text-primary">
          You don't seem to have any project assigned yet.
        </span>
      </Fragment>
    );
  }

  const createNewProject = (
    <Fragment>
      <div className="yourproject">
        <i className="fas fa-plus-circle" /> Create New Project
      </div>
      <form className="form" onSubmit={e => onCreateProject(e)}>
        <div className="form-group">
          <input
            type="text"
            placeholder="Project Name"
            name="projectNew"
            value={newProjectformData.project}
            onChange={e => onChange(e)}
          />
        </div>
        <input type="submit" className="btn btn-primary" value="Create" />
      </form>
    </Fragment>
  );

  const askToJoinExistingProject = (
    <div>
      <div className="yourproject">
        <i className="far fa-envelope-open"> </i> Pending Project invitations
      </div>
      <span className="normal text-primary">No invitations yet.</span>
    </div>
  );

  return (
    <Fragment>
      <div className="userWithNoProject">
        <div className="large">
          <span>Hello, </span>{" "}
          <span className="navdiv-projecttext">
            {userstate.userdata.user.name}
          </span>
        </div>
        {userProjects}
        {createNewProject}
        {askToJoinExistingProject}
      </div>
    </Fragment>
  );
};

DashboardUser.propTypes = {
  userstate: PropTypes.object,
  loading: PropTypes.bool,
  createProject: PropTypes.func.isRequired,
  setCurrentProject: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  { createProject, setCurrentProject }
)(DashboardUser);
