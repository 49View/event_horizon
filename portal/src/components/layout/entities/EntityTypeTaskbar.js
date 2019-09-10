import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  getEntitiesOfGroup,
  getFullEntity,
  changeEntitiesGroup
} from "../../../actions/entities";

import EntitiesThumbHandler from "./EntitiesThumbHandler";

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
  changeEntitiesGroup,
  entries,
  currentEntity
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
      <Fragment>
        <div
          className={
            selected
              ? "leftSideBarGroup leftSideBarGroupSelected"
              : "leftSideBarGroup"
          }
        >
          <span onClick={viewMore(text)}>
            <div className="leftSideBarIcon">
              <i className={icon} />
            </div>
            <div className="leftSideBarText"> {text}</div>
          </span>
        </div>
        {selected && (
          <EntitiesThumbHandler
            currentEntity={currentEntity}
            entries={entries}
            onClicked={getFullEntity}
          />
        )}
      </Fragment>
    );
  };

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

  return <Fragment>{topEntitySelectorBar}</Fragment>;
};

EntityTypeTaskBar.propTypes = {
  userstate: PropTypes.object,
  getEntitiesOfGroup: PropTypes.func.isRequired,
  changeEntitiesGroup: PropTypes.func.isRequired,
  entries: PropTypes.array,
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  userstate: state.auth,
  entries: state.entities.entriesFiltered,
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup, changeEntitiesGroup }
)(EntityTypeTaskBar);
