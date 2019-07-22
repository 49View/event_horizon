import React, { Fragment, useState, useEffect } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { getEntitiesOfGroup } from "../../actions/entities";
import Entries from "./entities/Entries";
import EntriesEditor from "./entities/EntriesEditor";
import { loadWasm } from "../../actions/wasm";

const ObjectsStrID = "Objects";
const MaterialsStrID = "Materials";
const ImagesStrID = "Images";
const FontsStrID = "Fonts";
const VectorsStrID = "Vectors";
const ColorsStrID = "Colors";

const Dashboard = ({
  userstate,
  loading,
  isWasmLoaded,
  getEntitiesOfGroup,
  loadWasm
}) => {
  const [currentGroup, setCurrentGroup] = useState("default");

  useEffect(() => {
    if (
      userstate.userdata &&
      userstate.userdata.project !== null &&
      currentGroup === "default"
    ) {
      getEntitiesOfGroup("material", userstate.userdata.project);
      if (!isWasmLoaded) {
        loadWasm("editor");
      }
    }
  }, [getEntitiesOfGroup, currentGroup, isWasmLoaded, userstate, loadWasm]);

  if (!userstate.isAuthenticated) {
    return <Redirect to="/" />;
  }

  const onRequestProject = e => {
    e.preventDefault();
    // login(email, password, project);
  };

  if (
    userstate.userdata &&
    (userstate.userdata.project === null || userstate.userdata.project === "")
  ) {
    return (
      <Fragment>
        <div className="userWithNoProject">
          <div className="large">
            <span>Hello, </span>{" "}
            <span className="navdiv-projecttext">
              {userstate.userdata.user.name}
            </span>
          </div>
          You don't seem to have any project assigned yet. Request access here:
          <form className="form" onSubmit={e => onRequestProject(e)}>
            <div className="form-group">
              <input type="text" placeholder="Project Name" name="project" />
            </div>
            <input
              type="submit"
              className="btn btn-primary"
              value="Request Access"
            />
          </form>
        </div>
      </Fragment>
    );
  }

  const viewMore = group => () => {
    let groupId = "";
    if (group === ObjectsStrID) groupId = "geom";
    if (group === MaterialsStrID) groupId = "material";
    if (group === ImagesStrID) groupId = "image";
    if (group === FontsStrID) groupId = "font";
    if (group === VectorsStrID) groupId = "profile";
    if (group === ColorsStrID) groupId = "color_scheme";
    setCurrentGroup(groupId);
    getEntitiesOfGroup(groupId, userstate.userdata.project);
  };

  const leftSideEntry = (icon, text, selected) => (
    <div
      className={
        selected
          ? "leftSideBarGroup leftSideBarGroupSelected"
          : "leftSideBarGroup"
      }
    >
      <a onClick={viewMore(text)} href="#!">
        <div className="leftSideBarIcon">
          <i className={icon} />
        </div>
        <div className="leftSideBarText"> {text}</div>
      </a>
    </div>
  );

  let proj = "";
  if (userstate.project && userstate.project !== "") {
    proj = userstate.project;
  } else if (userstate.userdata && userstate.userdata.project) {
    proj = userstate.userdata.project;
  }

  const projectNameBox = (
    <Fragment>
      <div className="project-a navdiv-projecttext">{proj}</div>
    </Fragment>
  );

  const topEntitySelectorBar = (
    <div className="topentityselectorbar-a topEntitySelectorBar">
      {leftSideEntry("fas fa-cube", ObjectsStrID, currentGroup === "geom")}
      {leftSideEntry(
        "fas fa-code-branch",
        MaterialsStrID,
        currentGroup === "material" || currentGroup === "default"
      )}
      {leftSideEntry("fas fa-images", ImagesStrID, currentGroup === "image")}
      {leftSideEntry("fas fa-font", FontsStrID, currentGroup === "font")}
      {leftSideEntry(
        "fas fa-vector-square",
        VectorsStrID,
        currentGroup === "profile"
      )}
      {leftSideEntry(
        "fas fa-brush",
        ColorsStrID,
        currentGroup === "colors_scheme"
      )}
    </div>
  );

  console.log("Dashboard render");
  return (
    <Fragment>
      <div className="dashboardContainer">
        {projectNameBox}
        {topEntitySelectorBar}
        <Entries />
        <EntriesEditor />
      </div>
    </Fragment>
  );
};

Dashboard.propTypes = {
  // setAlert: PropTypes.func.isRequired,
  userstate: PropTypes.object,
  loading: PropTypes.bool,
  isWasmLoaded: PropTypes.bool,
  getEntitiesOfGroup: PropTypes.func.isRequired,
  loadWasm: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  loading: state.entities.loading,
  isWasmLoaded: state.wasm.loaded
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup, loadWasm }
)(Dashboard);
