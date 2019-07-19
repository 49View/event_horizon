import React, { Fragment, useState, useEffect } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  getEntitiesOfGroup,
  updateEntriesPartialSearch
} from "../../actions/entities";
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
  loadWasm,
  updateEntriesPartialSearch
}) => {
  const [currentGroup, setCurrentGroup] = useState("default");

  useEffect(() => {
    if (
      userstate.userdata &&
      userstate.userdata.project !== null &&
      currentGroup === "default"
    ) {
      getEntitiesOfGroup("material", userstate.userdata.project);
    }
    if (!isWasmLoaded) {
      // console.log("load wasm");
      loadWasm("editor");
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

  const onChange = e => {
    updateEntriesPartialSearch(e.target.value.toLowerCase());
  };

  let proj = "";
  if (userstate.project && userstate.project !== "") {
    proj = userstate.project;
  } else if (userstate.userdata && userstate.userdata.project) {
    proj = userstate.userdata.project;
  }

  const searchBox = (
    <Fragment>
      <div className="project-a navdiv-projecttext">{proj}</div>

      <div className=" searchbar-a entitiesSearchBox">
        <input
          type="text"
          id="search-bar"
          placeholder="Search for..."
          onChange={e => onChange(e)}
        />
        <a href="#!" className="search-icon">
          <i className="fas fa-search" />
        </a>
      </div>
    </Fragment>
  );

  const leftSideBar = (
    <div className="sidebar-a leftSideBar">
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

  return (
    <Fragment>
      <div className="entitiesContainer">
        {searchBox}
        {leftSideBar}
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
  updateEntriesPartialSearch: PropTypes.func.isRequired,
  loadWasm: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  loading: state.entities.loading,
  isWasmLoaded: state.wasm.loaded
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup, updateEntriesPartialSearch, loadWasm }
)(Dashboard);
