import React, { useState, useEffect, Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import Spinner from "./Spinner";
// import ColorEditor from "./ColorEditor";
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
  userSessionId
}) => {
  let canvas = React.useRef(null);

  const [count, setCount] = useState(0);

  useEffect(() => {
    if (currentEntity !== null) {
      if (count === 0) {
        loadWasmComplete("editor", canvas.current, userToken, userSessionId);
        setCount(1);
      }
    }
  }, [canvas, count, userToken, userSessionId, currentEntity]);

  let mainEditorDiv = <Fragment />;

  const canvasStyle = {
    visibility: currentEntity ? "visible" : "hidden"
  };

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    group
  );

  mainEditorDiv = (
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
  userSessionId: PropTypes.string
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.entities.loading,
  group: state.entities.group,
  userToken: state.auth.token,
  userSessionId: state.auth.userdata
    ? state.auth.session
      ? state.auth.session
      : state.auth.userdata.session
    : null
});

export default connect(
  mapStateToProps,
  {}
)(DashboardProject);
