import React, { useEffect } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import ImageEditor from "./entities/ImageEditor";
import AppEditor from "./entities/AppEditor";
import GUIEditor from "./entities/GUIEditor";
import GeomEditor from "./entities/GeomEditor";
import MaterialEditor from "./entities/MaterialEditor";
import { wasmSetCanvasSize } from "../../actions/wasm";
import EntityUpdateContent from "./entities/EntityUpdateContent";
import EntityMetaSection from "./entities/EntityMetaSection";
import RenderParamsToolbar from "./entities/RenderParamsToolbar";
import { getFullEntity } from "../../actions/entities";
import {
  groupHasRenderToolbar,
  groupHasUpdateFacility,
  groupHasMetadataSection,
  GroupGeom,
  GroupMaterial,
  GroupImage,
  GroupUI,
  GroupScript
} from "../../utils/utils";

import store from "../../store";

const containerClassFromGroup = (currEntity, group) => {
  switch (group) {
    case GroupGeom:
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <GeomEditor />
      };
    case GroupMaterial:
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <MaterialEditor />
      };
    case GroupImage:
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <ImageEditor />
      };
    case GroupScript:
      return {
        mainContainerClass: "AppEditorRenderGrid",
        mainContainerDiv: <AppEditor />
      };
    case GroupUI:
      return {
        mainContainerClass: "GUIEditorRenderGrid",
        mainContainerDiv: <GUIEditor />
      };
    default:
      return {
        mainContainerClass: "AppEditorRenderGrid",
        mainContainerDiv: <AppEditor />
      };
  }
};

const DashboardProject = ({
  resize,
  currentEntity,
  entities,
  group,
  userData
}) => {
  let canvasContainer = React.useRef(null);

  useEffect(() => {
    // Shortcut to go straight to app/coding from the outset for most projects
    if (group === GroupScript && entities.length === 1 && !currentEntity) {
      store.dispatch(getFullEntity(entities[0]));
    }
  }, [currentEntity, entities, group]);

  if (!userData || !userData.project) {
    return <Redirect to="/" />;
  }

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    currentEntity,
    group
  );

  const bUseRenderParams = groupHasRenderToolbar(currentEntity, group);
  const bUseEntityUpdate = groupHasUpdateFacility(currentEntity, group);
  const bShowMetaSection = groupHasMetadataSection(currentEntity, group);

  if (canvasContainer.current) {
    const rect = canvasContainer.current.getBoundingClientRect();
    store.dispatch(wasmSetCanvasSize(rect));
  }

  const mainEditorDiv = (
    <div className={mainContainerClass}>
      {bUseRenderParams && <RenderParamsToolbar />}
      {bUseEntityUpdate && <EntityUpdateContent />}
      <div className="EntryEditorRender" ref={canvasContainer}></div>
      {currentEntity && mainContainerDiv}
      {bShowMetaSection && <EntityMetaSection />}
    </div>
  );

  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <div className="editor-a">{mainEditorDiv}</div>
    </div>
  );
};

DashboardProject.propTypes = {
  resize: PropTypes.bool,
  currentEntity: PropTypes.object,
  entities: PropTypes.array,
  group: PropTypes.string,
  userData: PropTypes.object
};

const mapStateToProps = state => ({
  resize: state.wasm.resize,
  currentEntity: state.entities.currentEntity,
  entities: state.entities.entries,
  group: state.entities.group,
  userData: state.auth.userdata
});

export default connect(
  mapStateToProps,
  {}
)(DashboardProject);
