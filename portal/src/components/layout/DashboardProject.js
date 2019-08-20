import React, { useState, useEffect } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import Spinner from "./Spinner";
import ImageEditor from "./entities/ImageEditor";
import AppEditor from "./entities/AppEditor";
import GeomEditor from "./entities/GeomEditor";
import MaterialEditor from "./entities/MaterialEditor";
import { loadWasmComplete } from "../../actions/wasm";
import EntityUpdateContent from "./entities/EntityUpdateContent";
import EntityMetaSection from "./entities/EntityMetaSection";

const containerClassFromGroup = group => {
  switch (group) {
    case "geom":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <GeomEditor />
      };
    case "material":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <MaterialEditor />
      };
    case "image":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <ImageEditor />
      };
    case "app":
      return {
        mainContainerClass: "AppEditorRenderGrid",
        mainContainerDiv: <AppEditor />
      };
    default:
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <GeomEditor />
      };
  }
};

const DashboardProject = ({
  currentEntity,
  loading,
  group,
  userToken,
  userData
}) => {
  let canvas = React.useRef(null);

  const [count, setCount] = useState(0);

  useEffect(() => {
    if (count === 0 && userData) {
      loadWasmComplete("editor", canvas.current, userToken, userData.session);
      setCount(1);
    }
  }, [canvas, count, userToken, userData]);

  if (!userData || !userData.project) {
    return <Redirect to="/" />;
  }

  const canvasVisibility =
    currentEntity &&
    (group === "app" || group === "geom" || group === "material")
      ? "visible"
      : "hidden";

  const entityUpdateDivVisible = currentEntity && group !== "app";

  const canvasSizeXNum = entityUpdateDivVisible ? 522 : 480;
  const canvasSizeYNum = entityUpdateDivVisible ? 522 : 270;

  const canvasSizeX = canvasSizeXNum.toString() + "px";
  const canvasSizeY = canvasSizeYNum.toString() + "px";

  const canvasClientSizeX =
    (canvasSizeXNum * (window.devicePixelRatio || 1)).toString() + "px";
  const canvasClientSizeY =
    (canvasSizeYNum * (window.devicePixelRatio || 1)).toString() + "px";

  const canvasPadding = entityUpdateDivVisible ? "5px" : "0px";
  const canvasMargin = entityUpdateDivVisible ? "7px" : "7px";

  const canvasStyle = {
    visibility: canvasVisibility,
    width: canvasSizeX,
    height: canvasSizeY,
    margin: canvasMargin,
    padding: canvasPadding
  };

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    group
  );

  // const entityBased = group !== "app";

  const mainEditorDiv = (
    <div className={mainContainerClass}>
      {entityUpdateDivVisible && (
        <div className="nameValue-a medium text-primary">
          {currentEntity && currentEntity.entity.metadata.name}
        </div>
      )}
      {entityUpdateDivVisible && <EntityUpdateContent />}
      <div className="EntryEditorRender">
        <canvas
          width={canvasClientSizeX}
          height={canvasClientSizeY}
          style={canvasStyle}
          ref={canvas}
          className="Canvas"
          onContextMenu={e => e.preventDefault()}
        />
      </div>
      {currentEntity && mainContainerDiv}
      {currentEntity && entityUpdateDivVisible && <EntityMetaSection />}
    </div>
  );

  return (
    <div className="dashboardContainer">
      {loading && <Spinner />}
      <Entries cname="thumbs-a thumbsEntityArea" />
      <div className="editor-a entryEditor">{mainEditorDiv}</div>
    </div>
  );
};

DashboardProject.propTypes = {
  currentEntity: PropTypes.object,
  loading: PropTypes.bool,
  group: PropTypes.string,
  userToken: PropTypes.string,
  userData: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.entities.loading,
  group: state.entities.group,
  userToken: state.auth.token,
  userData: state.auth.userdata
});

export default connect(
  mapStateToProps,
  {}
)(DashboardProject);
