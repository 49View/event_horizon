import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  getEntitiesOfGroup,
  getFullEntity,
  changeEntitiesGroup
} from "../../../actions/entities";
import {
  GroupMaterial,
  GroupGeom,
  GroupImage,
  GroupUI,
  GroupProfile,
  GroupFont,
  GroupScript
} from "../../../utils/utils";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import EntityDragAndImport from "./EntityDragAndImport";

// const AppStrID = "Apps";
const ScriptStrID = "Scripts";
const ObjectsStrID = "Objects";
const MaterialsStrID = "Materials";
const ImagesStrID = "Images";
const FontsStrID = "Fonts";
const GUIsStrID = "UIs";
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
      const groupId = GroupScript;
      setCurrentGroup(groupId);
      getEntitiesOfGroup(groupId, userstate.userdata.project);
    }
  }, [getEntitiesOfGroup, currentGroup, userstate]);

  const viewMore = group => () => {
    let groupId = "";
    if (group === ScriptStrID) groupId = GroupScript;
    if (group === ObjectsStrID) groupId = GroupGeom;
    if (group === MaterialsStrID) groupId = GroupMaterial;
    if (group === ImagesStrID) groupId = GroupImage;
    if (group === GUIsStrID) groupId = GroupUI;
    if (group === FontsStrID) groupId = GroupFont;
    if (group === VectorsStrID) groupId = GroupProfile;
    if (group === ColorsStrID) groupId = "color_scheme";
    if (currentGroup !== groupId) {
      setCurrentGroup(groupId);
      changeEntitiesGroup(groupId, userstate.userdata.project);
      // return <Redirect to="/dashboard/material" />;
    }
  };

  const topSideEntry = (icon, text, selected) => {
    return (
      <div className="leftSideBarGroupContainer">
        <div
          className={
            selected
              ? "leftSideBarGroup leftSideBarGroupSelected"
              : "leftSideBarGroup"
          }
        >
          <span onClick={viewMore(text)}>
            <span className="leftSideBarIcon">
              <i className={icon} />
            </span>
            <span className="leftSideBarText"> {text}</span>
          </span>
        </div>
        {selected && (
          <Fragment>
            <EntityDragAndImport />
            <EntitiesThumbHandler
              currentEntity={currentEntity}
              entries={entries}
              onClicked={getFullEntity}
            />
          </Fragment>
        )}
      </div>
    );
  };

  const topEntitySelectorBar = (
    <div className="topentityselectorbar-a topEntitySelectorBar">
      {topSideEntry("fas fa-rocket", ScriptStrID, currentGroup === GroupScript)}
      {topSideEntry("fas fa-cube", ObjectsStrID, currentGroup === GroupGeom)}
      {topSideEntry(
        "fas fa-code-branch",
        MaterialsStrID,
        currentGroup === GroupMaterial
      )}
      {topSideEntry("fas fa-images", ImagesStrID, currentGroup === GroupImage)}
      {topSideEntry("fas fa-bars", GUIsStrID, currentGroup === GroupUI)}
      {topSideEntry("fas fa-font", FontsStrID, currentGroup === GroupFont)}
      {topSideEntry(
        "fas fa-vector-square",
        VectorsStrID,
        currentGroup === GroupProfile
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
