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
import RenderParamsToolbar from "./entities/RenderParamsToolbar";

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

  const bShowEntityCanvas = currentEntity && group !== "app";

  const canvasSizeXNum = bShowEntityCanvas ? 529 : 510;
  const canvasSizeYNum = bShowEntityCanvas ? 536 : 286;

  const canvasSizeX = canvasSizeXNum.toString() + "px";
  const canvasSizeY = canvasSizeYNum.toString() + "px";

  const canvasClientSizeX =
    (canvasSizeXNum * (window.devicePixelRatio || 1)).toString() + "px";
  const canvasClientSizeY =
    (canvasSizeYNum * (window.devicePixelRatio || 1)).toString() + "px";

  const canvasPadding = bShowEntityCanvas ? "5px" : "1px";
  const canvasMargin = bShowEntityCanvas ? "0px" : "0px";
  const canvasRadius = bShowEntityCanvas ? "10px" : "3px";

  const canvasStyle = {
    visibility: canvasVisibility,
    width: canvasSizeX,
    height: canvasSizeY,
    margin: canvasMargin,
    padding: canvasPadding,
    borderRadius: canvasRadius
  };

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    group
  );

  // const entityBased = group !== "app";

  const mainEditorDiv = (
    <div className={mainContainerClass}>
      {bShowEntityCanvas && <RenderParamsToolbar />}
      {bShowEntityCanvas && <EntityUpdateContent />}
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
      {currentEntity && bShowEntityCanvas && <EntityMetaSection />}
    </div>
  );

  return (
    <div className="dashboardContainer">
      {loading && <Spinner />}
      <Entries cname="thumbs-a thumbsEntityArea" />
      <div className="editor-a">{mainEditorDiv}</div>
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
