import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const MaterialEditor = ({ currentEntity }) => {
  let mainContent = <Fragment />;
  if (currentEntity.jsonRet.values.mType === "PN_SH") {
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
          {/* <canvas
            ref={canvasRef2}
            className="CanvasMaterial"
            onContextMenu={e => e.preventDefault()}
          /> */}
        </div>
      </div>
    );
  }

  return <Fragment>{mainContent}</Fragment>;
};

MaterialEditor.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  {}
)(MaterialEditor);
