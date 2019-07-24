import React, { Fragment, useEffect, useState } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";
import { loadWasmComplete } from "../../../actions/wasm";

const GeomEditor = ({
  currentEntity,
  currentEntityNodes,
  loading,
  group,
  isWasmLoaded,
  userToken,
  userSessionId,
  showConfirmAlert
}) => {
  const canvasRef = React.useRef(null);
  const [count, setCount] = useState(0);

  useEffect(() => {
    if (currentEntity !== null) {
      if (count === 0) {
        loadWasmComplete("editor", canvasRef.current, userToken, userSessionId);
        setCount(1);
      }
    }
  }, [count, userToken, userSessionId, currentEntity]);

  if (userSessionId === null) {
    return <Redirect to="/" />;
  }

  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
  };

  const onReplaceEntity = e => {};

  let mainContent = <Fragment />;

  mainContent = (
    <Fragment>
      <div className="EntryEditorRender">
        <canvas
          id="#canvas"
          ref={canvasRef}
          className="Canvas"
          onContextMenu={e => e.preventDefault()}
        />
      </div>
      <div className="nodeViewer-a">
        {currentEntityNodes &&
          currentEntityNodes.mrefs.map(e => (
            <div key={e.key} className="smallObjectMaterial">
              {e.key}
              {e.value.values.mV3fs[0].key}
              <input
                type="button"
                className="btn2 btn-primary"
                value="Replace"
                name={e.key}
                onClick={eb => onReplaceEntity(eb)}
              />
            </div>
          ))}
      </div>
    </Fragment>
  );

  const entityRender =
    currentEntity === null ? (
      <div className="EntryEditorRender" />
    ) : (
      <div className="GeomEditorRenderGrid">
        <div className="nameValue-a medium text-primary">
          {currentEntity.entity.metadata.name}
        </div>
        <EntityUpdateContent />
        {mainContent}
        <EntityTags />
        <EntityInfo />
        <ConfirmAlert />
        <div />
        <div className="deleteentity-a">
          <input
            type="button"
            className="btn2 btn-danger"
            value="Delete :'("
            onClick={e => onDeleteEntity(e)}
          />
        </div>
      </div>
    );

  return (
    <Fragment>
      {loading && <Spinner />}
      <div className="editor-a entryEditor">{entityRender}</div>
    </Fragment>
  );
};

GeomEditor.propTypes = {
  currentEntity: PropTypes.object,
  currentEntityNodes: PropTypes.object,
  loading: PropTypes.bool,
  isWasmLoaded: PropTypes.bool,
  isWasmRunning: PropTypes.bool,
  group: PropTypes.string,
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
  showConfirmAlert: PropTypes.func.isRequired,
  wasmCanvas: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  currentEntityNodes: state.entities.currentEntityNodes,
  loading: state.entities.loading,
  userToken: state.auth.token,
  userSessionId: state.auth.userdata
    ? state.auth.session
      ? state.auth.session
      : state.auth.userdata.session
    : null,
  group: state.entities.group,
  isWasmLoaded: state.wasm.loaded,
  isWasmRunning: state.wasm.running,
  wasmCanvas: state.wasm.wasmCanvas
});

export default connect(
  mapStateToProps,
  { showConfirmAlert }
)(GeomEditor);
