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

const rgbToHex = (r, g, b) =>
  "#" +
  [r, g, b]
    .map(x => {
      const hex = x.toString(16);
      return hex.length === 1 ? "0" + hex : hex;
    })
    .join("");

const GeomEditor = ({
  currentEntity,
  currentEntityNodes,
  loading,
  replaceMaterialOn,
  userToken,
  userSessionId
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

  const onReplaceEntity = e => {
    store.dispatch({ type: SET_SELECTED_MAT_NAME, payload: e.target.name });
  };

  const onChangeMaterialColor = e => {
    // store.dispatch({ type: SET_SELECTED_MAT_NAME, payload: e.target.name });
  };

  const onChangeMaterialMetalness = e => {
    // store.dispatch({ type: SET_SELECTED_MAT_NAME, payload: e.target.name });
  };

  let mainContent = <Fragment />;

  let matSimples = [];

  if (currentEntityNodes) {
    for (const e of currentEntityNodes.mrefs) {
      let matSimpleEntry = {
        key: e.key,
        baseColor: "#ff",
        baseTexture: ""
      };
      if (e.value.values.mV3fs[0].key === "diffuseColor") {
        const rgbC = e.value.values.mV3fs[0].value;
        matSimpleEntry.baseColor = rgbToHex(
          rgbC[0] * 255,
          rgbC[1] * 255,
          rgbC[2] * 255
        );
      }
      for (const tn of e.value.values.mStrings) {
        if (tn.key === "diffuseColor") {
          matSimpleEntry.baseTexture = tn.value;
        }
      }
      matSimples.push(matSimpleEntry);
    }
  }

  const InObjectMaterials = () => {
    return (
      <Fragment>
        <div className="nodeViewer-a">
          <div className="medium text-primary titleMargin">Materials</div>
          {matSimples.map(e => (
            <div key={e.key} className="smallObjectMaterial">
              <div className="matName-a">{e.key}</div>
              <div className="replaceMat-a">
                <span name={e.key} onClick={eb => onReplaceEntity(eb)}>
                  <i className="fas fa-exchange-alt" />
                </span>
              </div>

              <div className="baseColorLabel-a">Color</div>
              <div className="baseColorPicker-a">
                <input
                  type="color"
                  name="head"
                  defaultValue={e.baseColor}
                  onChange={eb => onChangeMaterialColor(eb)}
                />
              </div>

              <div className="metallicLabel-a">Metallic</div>
              <div className="metallicSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue="25"
                  name="metallic"
                  id="metallicSlider"
                  onChange={eb => onChangeMaterialMetalness(eb)}
                />
              </div>

              <div className="roughnessLabel-a">Roughness</div>
              <div className="roughnessSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue="25"
                  name="roughness"
                  id="roughnessSlider"
                  onChange={eb => onChangeMaterialMetalness(eb)}
                />
              </div>

              <div className="aoLabel-a">Roughness</div>
              <div className="aoSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue="25"
                  name="ao"
                  id="aoSlider"
                  onChange={eb => onChangeMaterialMetalness(eb)}
                />
              </div>

              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src="/empty.png" alt="B" />
              </div>

              <div className="baseTextureLabel-a">Base</div>
              <div className="baseTextureLabel-a">Normal</div>
              <div className="baseTextureLabel-a">Metal</div>
              <div className="baseTextureLabel-a">Rough</div>
              <div className="baseTextureLabel-a">AO</div>
              <div className="baseTextureLabel-a">Emissive</div>
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
  replaceMaterialOn: PropTypes.bool,
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
  wasmCanvas: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  currentEntityNodes: state.entities.currentEntityNodes,
  loading: state.entities.loading,
  replaceMaterialOn: state.entities.replaceMaterialOn,
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
