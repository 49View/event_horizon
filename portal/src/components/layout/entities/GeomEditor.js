import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import SmallEntriesDialog from "./SmallEntriesDialog";
import { SET_MODAL_SELECTED_ENTITY_NAME } from "../../../actions/types";
import store from "../../../store";
import {
  changeMaterialPropery,
  replaceMaterial
} from "../../../actions/entities";
import { fillMaterialParams } from "../../../utils/materialUtils";
import { GroupMaterial } from "../../../utils/utils";

const GeomEditor = ({
  currentEntity,
  currentEntityNodes,
  smallEntityModalOn,
  changeMaterialPropery
}) => {
  const onReplaceEntity = e => {
    store.dispatch({
      type: SET_MODAL_SELECTED_ENTITY_NAME,
      payload: e.currentTarget.dataset.id
    });
  };

  const onChangeMaterialPropery = e => {
    changeMaterialPropery(e);
  };

  let matSimples = [];

  if (currentEntityNodes) {
    // eslint-disable-next-line
    for (const e of currentEntityNodes.mrefs) {
      matSimples.push(
        fillMaterialParams(e.key, e.value.values, e.value.thumbValues)
      );
    }
  }

  const InObjectMaterials = () => {
    return (
      <Fragment>
        <div className="nodeViewer-a">
          <div className="titleMargin">Materials</div>
          {matSimples.map(e => (
            <div key={e.key} className="smallObjectMaterial">
              <div className="matName-a">{e.key}</div>
              <div className="replaceMat-a">
                <span data-id={e.key} onClick={eb => onReplaceEntity(eb)}>
                  <i className="fas fa-exchange-alt" />
                </span>
              </div>

              <div className="baseColorLabel-a">Color</div>
              <div className="baseColorPicker-a">
                <input
                  type="color"
                  id={e.key}
                  name="diffuseColor-hexcolor"
                  defaultValue={e.baseColor}
                  onChange={eb => onChangeMaterialPropery(eb)}
                />
              </div>

              <div className="metallicLabel-a">Metallic</div>
              <div className="metallicSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue={e.metallicValue * 100}
                  id={e.key}
                  name="metallicV-float100"
                  onChange={eb => onChangeMaterialPropery(eb)}
                />
              </div>

              <div className="roughnessLabel-a">Roughness</div>
              <div className="roughnessSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue={e.roughnessValue * 100}
                  name="roughnessV-float100"
                  id={e.key}
                  onChange={eb => onChangeMaterialPropery(eb)}
                />
              </div>

              <div className="aoLabel-a">Ambient Occlusion</div>
              <div className="aoSlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue={e.aoValue * 100}
                  name="aoV-float100"
                  id={e.key}
                  onChange={eb => onChangeMaterialPropery(eb)}
                />
              </div>

              <div className="opacityLabel-a">Opacity</div>
              <div className="opacitySlider-a">
                <input
                  type="range"
                  className="pbrSlider"
                  min="1"
                  max="100"
                  defaultValue={e.opacityValue * 100}
                  name="opacity-float100"
                  id={e.key}
                  onChange={eb => onChangeMaterialPropery(eb)}
                />
              </div>

              <div className="baseTexture-a">
                <img src={e.diffuseTexture} alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src={e.normalTexture} alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src={e.metallicTexture} alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src={e.roughnessTexture} alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src={e.aoTexture} alt="B" />
              </div>
              <div className="baseTexture-a">
                <img src={e.opacityTexture} alt="B" />
              </div>

              <div className="baseTextureLabel-a">Base</div>
              <div className="baseTextureLabel-a">Normal</div>
              <div className="baseTextureLabel-a">Metal</div>
              <div className="baseTextureLabel-a">Rough</div>
              <div className="baseTextureLabel-a">AO</div>
              <div className="baseTextureLabel-a">Opacity</div>
            </div>
          ))}
        </div>
      </Fragment>
    );
  };

  return (
    <Fragment>
      {smallEntityModalOn && (
        <SmallEntriesDialog
          group={GroupMaterial}
          onClickCallback={replaceMaterial}
        />
      )}
      <div className="source_tabs-a">
        <div className="source_tabs-internal">
          {currentEntity.entity.metadata.name}
        </div>
      </div>

      <InObjectMaterials />
    </Fragment>
  );
};

GeomEditor.propTypes = {
  currentEntity: PropTypes.object,
  currentEntityNodes: PropTypes.object,
  smallEntityModalOn: PropTypes.bool,
  changeMaterialPropery: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  currentEntityNodes: state.entities.currentEntityNodes,
  smallEntityModalOn: state.entities.smallEntityModalOn
});

export default connect(
  mapStateToProps,
  { changeMaterialPropery }
)(GeomEditor);
