import React, { Fragment, useEffect, useState } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import EntityUpdateContent from "./EntityUpdateContent";
import { loadWasmComplete } from "../../../actions/wasm";
import SmallEntriesDialog from "./SmallEntriesDialog";
import EntityMetaSection from "./EntityMetaSection";
import { SET_SELECTED_MAT_NAME } from "../../../actions/types";
import store from "../../../store";

const GeomEditor = ({
  currentEntity,
  currentEntityNodes,
  loading,
  userToken,
  userSessionId
}) => {
  const canvasRef = React.useRef(null);
  const [count, setCount] = useState(0);
  const [replaceMaterialOn, setReplaceMaterialOn] = useState(false);

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

  const onReplaceEntity = e => {
    store.dispatch({ type: SET_SELECTED_MAT_NAME, payload: e.target.name });
    setReplaceMaterialOn(true);
  };

  let mainContent = <Fragment />;

  const InObjectMaterials = () => {
    return (
      <Fragment>
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
  };

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
      {replaceMaterialOn ? <SmallEntriesDialog /> : <InObjectMaterials />}
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
        <EntityMetaSection />
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
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
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
  wasmCanvas: state.wasm.wasmCanvas
});

export default connect(
  mapStateToProps,
  {}
)(GeomEditor);
