import React, { Fragment, useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";
import { runWasm } from "../../../actions/wasm";

let localWasmcanvas = null;

const EntriesEditor = ({
  currentEntity,
  loading,
  group,
  isWasmLoaded,
  isWasmRunning,
  userToken,
  userSessionId,
  showConfirmAlert,
  runWasm
}) => {
  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
  };

  const canvasRef = React.useRef(null);
  // const [count, setCount] = useState(0);

  useEffect(() => {
    if (
      (group === "geom" || group === "material") &&
      currentEntity !== null &&
      !loading
      // !isWasmRunning
    ) {
      // if (count % 2 === 0) {
      console.log(currentEntity);
      window.Module = {
        arguments: [userToken, userSessionId],
        print: text => {
          console.log("W: " + text);
        },
        printErr: text => {
          console.log("W ERROR: " + text);
        },
        canvas: canvasRef.current,
        onRuntimeInitialized: () => {
          console.log("Runtime initialized");

          // dispatch(wasmRunSuccess(canvas));
        },
        instantiateWasm: (imports, successCallback) => {
          WebAssembly.instantiate(window.wasmBinary, imports)
            .then(function(output) {
              console.log("wasm instantiation succeeded");
              successCallback(output.instance);
            })
            .catch(function(e) {
              console.log("wasm instantiation failed! " + e);
              // dispatch(wasmRunFailed(e.message));
            });
          return {};
        },
        destroy: cpp => {
          console.log("destroy");
        }
      };

      // if (count === 0) {
      const s = document.createElement("script");
      s.text = window.wasmScript;
      document.body.appendChild(s);
      // setCount(1);
      // }

      //        runWasm(canvasRef.current, userToken, userSessionId);
    }
    // if (
    //   group === "geom" &&
    //   isWasmLoaded &&
    //   currentEntity !== null &&
    //   isWasmRunning
    // ) {
    //   reCanvas(localWasmcanvas);
    // }
  });

  // useEffect(() => {});

  // {e => (localWasmcanvas = e)}
  // setCount(count + 1);

  let mainContent = <Fragment />;
  if (currentEntity !== null) {
    if (group === "geom") {
      console.log("localCanvas ", localWasmcanvas, " ref ", canvasRef);
      mainContent = (
        <div className="EntryEditorRender">
          <canvas
            id="#canvas"
            ref={canvasRef}
            //ref={useRef("#canvas")}
            // ref={e => (localWasmcanvas = e)}
            className="Canvas"
            onContextMenu={e => e.preventDefault()}
          />
        </div>
      );
    }
    if (group === "image") {
      mainContent = (
        <div className="EntryEditorRender">
          <img className="bigimagequad" src={currentEntity.blobURL} alt="" />
        </div>
      );
    }
    if (group === "material") {
      if (currentEntity.jsonRet.values.mType === "PN_SH") {
        // Base Color
        let depPBRMap = {
          albedoTexture: "empty.png",
          normalTexture: "empty.png",
          roughnessTexture: "empty.png",
          metallicTexture: "empty.png",
          aoTexture: "empty.png",
          heightTexture: "empty.png",
          opacityTexture: "empty.png",
          translucencyTexture: "empty.png"
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
                ref={e => (localWasmcanvas = e)}
                className="CanvasMaterial"
                onContextMenu={e => e.preventDefault()}
              />
            </div>
          </div>
        );
      }
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

  return loading ? (
    <Spinner />
  ) : (
    <Fragment>
      <div className="editor-a entryEditor">{entityRender}</div>
    </Fragment>
  );
};

EntriesEditor.propTypes = {
  currentEntity: PropTypes.object,
  loading: PropTypes.bool,
  isWasmLoaded: PropTypes.bool,
  isWasmRunning: PropTypes.bool,
  group: PropTypes.string,
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
  showConfirmAlert: PropTypes.func.isRequired,
  runWasm: PropTypes.func.isRequired,
  wasmCanvas: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.entities.loading,
  userToken: state.auth.token,
  userSessionId: state.auth.session
    ? state.auth.session
    : state.auth.userdata.session,
  group: state.entities.group,
  isWasmLoaded: state.wasm.loaded,
  isWasmRunning: state.wasm.running,
  wasmCanvas: state.wasm.wasmCanvas
});

// const mapDispatchToProps = dispatch => {
//   return {
//     onRunWasm: canvas => dispatch(actions.runWasm(canvas))
//   };
// };

export default connect(
  mapStateToProps,
  { showConfirmAlert, runWasm }
)(EntriesEditor);
