import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  getEntitiesOfGroup,
  changeEntitiesGroup
} from "../../../actions/entities";

const AppStrID = "Apps";
const ObjectsStrID = "Objects";
const MaterialsStrID = "Materials";
const ImagesStrID = "Images";
const FontsStrID = "Fonts";
const GUIsStrID = "GUIs";
const VectorsStrID = "Vectors";
const ColorsStrID = "Colors";

const EntityTypeTaskBar = ({
  userstate,
  getEntitiesOfGroup,
  changeEntitiesGroup
}) => {
  const [currentGroup, setCurrentGroup] = useState("default");

  useEffect(() => {
    if (
      userstate.userdata &&
      userstate.userdata.project !== null &&
      currentGroup === "default"
    ) {
      const groupId = "app";
      setCurrentGroup(groupId);
      getEntitiesOfGroup(groupId, userstate.userdata.project);
    }
  }, [getEntitiesOfGroup, currentGroup, userstate]);

  const viewMore = group => () => {
    let groupId = "";
    if (group === AppStrID) groupId = "app";
    if (group === ObjectsStrID) groupId = "geom";
    if (group === MaterialsStrID) groupId = "material";
    if (group === ImagesStrID) groupId = "image";
    if (group === GUIsStrID) groupId = "ui";
    if (group === FontsStrID) groupId = "font";
    if (group === VectorsStrID) groupId = "profile";
    if (group === ColorsStrID) groupId = "color_scheme";
    if (currentGroup !== groupId) {
      setCurrentGroup(groupId);
      changeEntitiesGroup(groupId, userstate.userdata.project);
      // return <Redirect to="/dashboard/material" />;
    }
  };

  const topSideEntry = (icon, text, selected) => {
    return (
      <div
        className={
          selected
            ? "leftSideBarGroup leftSideBarGroupSelected"
            : "leftSideBarGroup"
        }
      >
        <Fragment>
          <span onClick={viewMore(text)}>
            <div className="leftSideBarIcon">
              <i className={icon} />
            </div>
            <div className="leftSideBarText"> {text}</div>
          </span>
        </Fragment>
      </div>
    );
  };

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
      {topSideEntry("fas fa-rocket", AppStrID, currentGroup === "app")}
      {topSideEntry("fas fa-cube", ObjectsStrID, currentGroup === "geom")}
      {topSideEntry(
        "fas fa-code-branch",
        MaterialsStrID,
        currentGroup === "material"
      )}
      {topSideEntry("fas fa-images", ImagesStrID, currentGroup === "image")}
      {topSideEntry("fas fa-bars", GUIsStrID, currentGroup === "ui")}
      {topSideEntry("fas fa-font", FontsStrID, currentGroup === "font")}
      {topSideEntry(
        "fas fa-vector-square",
        VectorsStrID,
        currentGroup === "profile"
      )}
      {topSideEntry(
        "fas fa-brush",
        ColorsStrID,
        currentGroup === "colors_scheme"
      )}
    </div>
  );

  return (
    <Fragment>
      <div className="projetTaskbar">
        {projectNameBox}
        {topEntitySelectorBar}
      </div>
    </Fragment>
  );
};

EntityTypeTaskBar.propTypes = {
  userstate: PropTypes.object,
  getEntitiesOfGroup: PropTypes.func.isRequired,
  changeEntitiesGroup: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup, changeEntitiesGroup }
)(EntityTypeTaskBar);
