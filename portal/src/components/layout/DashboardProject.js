import React, { useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import Spinner from "./Spinner";
// import ColorEditor from "./ColorEditor";
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
    // if (currentEntity !== null) {
    if (count === 0 && userData) {
      loadWasmComplete("editor", canvas.current, userToken, userData.session);
      setCount(1);
    }
    // }
  }, [canvas, count, userToken, userData]);

  const canvasStyle = {
    visibility: currentEntity ? "visible" : "hidden"
  };

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    group
  );

  const mainEditorDiv = (
    <div className={mainContainerClass}>
      <div className="nameValue-a medium text-primary">
        {currentEntity && currentEntity.entity.metadata.name}
      </div>
      {currentEntity && <EntityUpdateContent />}
      <div className="EntryEditorRender">
        <canvas
          style={canvasStyle}
          ref={canvas}
          className="Canvas"
          onContextMenu={e => e.preventDefault()}
        />
      </div>
      {currentEntity && mainContainerDiv}
      {currentEntity && <EntityMetaSection />}
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
