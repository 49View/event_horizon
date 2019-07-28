import React, { Fragment, useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import EntityUpdateContent from "./EntityUpdateContent";
import { loadWasmComplete } from "../../../actions/wasm";
import EntityMetaSection from "./EntityMetaSection";
import { changeMaterialPropery } from "../../../actions/entities";

const MaterialEditor = ({
  currentEntity,
  currentEntityNodes,
  loading,
  replaceMaterialOn,
  userToken,
  userSessionId,
  changeMaterialPropery
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

  let mainContent = <Fragment />;
  if (currentEntity && currentEntity.jsonRet.values.mType === "PN_SH") {
    // Base Color
    let depPBRMap = {
      albedoTexture: "/empty.png",
      normalTexture: "/empty.png",
      roughnessTexture: "/empty.png",
      metallicTexture: "/empty.png",
      aoTexture: "/empty.png",
      heightTexture: "/empty.png",
      opacityTexture: "/empty.png",
      translucencyTexture: "/empty.png"
    };

    for (const dep of currentEntity.jsonRet.values.mStrings) {
      if (dep.key === "diffuseTexture") {
        depPBRMap.albedoTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "normalTexture") {
        depPBRMap.normalTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "roughnessTexture") {
        depPBRMap.roughnessTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "metallicTexture") {
        depPBRMap.metallicTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "aoTexture") {
        depPBRMap.aoTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "heightTexture") {
        depPBRMap.heightTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "opacityTexture") {
        depPBRMap.opacityTexture = currentEntity.deps[dep.value];
      }
      if (dep.key === "translucencyTexture") {
        depPBRMap.translucencyTexture = currentEntity.deps[dep.value];
      }
    }

    mainContent = (
      <div className="materialPBRContainer">
        <div className="materialPBRRaw">
          <div className="mediumPBRquad">
            <img src={depPBRMap.albedoTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Albedo
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.roughnessTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Roughness
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.aoTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Ambient Occlusion
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.heightTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Height
            </div>
          </div>
        </div>
        <div className="materialPBRRaw2">
          <div className="mediumPBRquad">
            <img src={depPBRMap.normalTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Normal
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.metallicTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Metallic
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.opacityTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Opacity
            </div>
          </div>
          <div className="mediumPBRquad">
            <img src={depPBRMap.translucencyTexture} alt="" />
            <div className="normal text-secondary text-center material-text">
              Translucency
            </div>
          </div>
        </div>
        <div className="materialRender">
          <canvas
            // ref={e => (localWasmcanvas = e)}
            className="CanvasMaterial"
            onContextMenu={e => e.preventDefault()}
          />
        </div>
      </div>
    );
  }

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

MaterialEditor.propTypes = {
  currentEntity: PropTypes.object,
  currentEntityNodes: PropTypes.object,
  loading: PropTypes.bool,
  replaceMaterialOn: PropTypes.bool,
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
  wasmCanvas: PropTypes.object,
  changeMaterialPropery: PropTypes.func.isRequired
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
  { changeMaterialPropery }
)(MaterialEditor);
