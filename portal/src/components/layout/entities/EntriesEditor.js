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

const EntriesEditor = ({
  currentEntity,
  currentEntityNodes,
  loading,
  group,
  isWasmLoaded,
  userToken,
  userSessionId,
  showConfirmAlert
}) => {
  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
  };

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

  let mainContent = <Fragment />;
  if (currentEntity !== null) {
    if (group === "image") {
    }
    if (group === "material") {
    }
  }

  const entityRender =
    currentEntity === null ? (
      <div className="EntryEditorRender" />
    ) : (
      <div className="EntryEditorRenderGrid">
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

EntriesEditor.propTypes = {
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
)(EntriesEditor);
