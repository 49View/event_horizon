import React, { Fragment } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  getEntitiesOfGroup,
  updateEntriesPartialSearch
} from "../../actions/entities";
import Entries from "./entities/Entries";
import EntriesEditor from "./entities/EntriesEditor";

const ObjectsStrID = "Objects";
const MaterialsStrID = "Materials";
const ImagesStrID = "Images";
const FontsStrID = "Fonts";
const VectorsStrID = "Vectors";
const ColorsStrID = "Colors";

const Dashboard = ({
  userstate,
  getEntitiesOfGroup,
  updateEntriesPartialSearch
}) => {
  // console.log(userstate);

  // useEffect(() => {
  //   if (userstate.userdata && userstate.userdata.project !== "") {
  //     getEntitiesOfGroup("image", userstate.userdata.project);
  //   }
  // }, [getEntitiesOfGroup, userstate]);

  if (!userstate.isAuthenticated) {
    return <Redirect to="/" />;
  }

  if (
    userstate.userdata &&
    (userstate.userdata.project === null || userstate.userdata.project === "")
  ) {
    return (
      <Fragment>
        <div>You don't seem to have any project assigned yet.</div>
      </Fragment>
    );
  }

  const viewMore = group => () => {
    let groupId = "undefined";
    if (group === ObjectsStrID) groupId = "geom";
    if (group === MaterialsStrID) groupId = "material";
    if (group === ImagesStrID) groupId = "image";
    if (group === FontsStrID) groupId = "font";
    if (group === VectorsStrID) groupId = "profile";
    if (group === ColorsStrID) groupId = "color_scheme";
    getEntitiesOfGroup(groupId, userstate.userdata.project);
    // console.log(group);
  };

  const leftSideEntry = (icon, text) => (
    <div className="leftSideBarGroup">
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
      <div className="project-navbar large">
        <span className="navdiv-projecttext">{proj}</span>
      </div>

      <div className="entitiesSearchBox">
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
    <div className="leftSideBar">
      {leftSideEntry("fas fa-cube", ObjectsStrID)}
      {leftSideEntry("fas fa-code-branch", MaterialsStrID)}
      {leftSideEntry("fas fa-images", ImagesStrID)}
      {leftSideEntry("fas fa-font", FontsStrID)}
      {leftSideEntry("fas fa-vector-square", VectorsStrID)}
      {leftSideEntry("fas fa-brush", ColorsStrID)}
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
  entities: PropTypes.array,
  getEntitiesOfGroup: PropTypes.func.isRequired,
  updateEntriesPartialSearch: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  entities: state.entities.entries
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup, updateEntriesPartialSearch }
)(Dashboard);
