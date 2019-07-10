import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";

const EntriesEditor = ({ currentEntity, loading, group, showConfirmAlert }) => {
  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
  };

  let mainContent = <Fragment />;
  if (currentEntity !== null) {
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
                <div className="normal text-secondary text-center">Albedo</div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.roughnessTexture} alt="" />
                <div className="normal text-secondary text-center">
                  Roughness
                </div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.aoTexture} alt="" />
                <div className="normal text-secondary text-center">
                  Ambient Occlusion
                </div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.heightTexture} alt="" />
                <div className="normal text-secondary text-center">Height</div>
              </div>
            </div>
            <div className="materialPBRRaw">
              <div className="mediumPBRquad">
                <img src={depPBRMap.normalTexture} alt="" />
                <div className="normal text-secondary text-center">Normal</div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.metallicTexture} alt="" />
                <div className="normal text-secondary text-center">
                  Metallic
                </div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.opacityTexture} alt="" />
                <div className="normal text-secondary text-center">Opacity</div>
              </div>
              <div className="mediumPBRquad">
                <img src={depPBRMap.translucencyTexture} alt="" />
                <div className="normal text-secondary text-center">
                  Translucency
                </div>
              </div>
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
  group: PropTypes.string,
  showConfirmAlert: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.entities.loading,
  group: state.entities.group
});

export default connect(
  mapStateToProps,
  { showConfirmAlert }
)(EntriesEditor);
