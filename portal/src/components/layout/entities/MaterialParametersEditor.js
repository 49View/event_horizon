import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { fillMaterialParams } from "../../../utils/materialUtils";
import { changeMaterialPropery } from "../../../actions/entities";

const MaterialParametersEditor = ({ entity, cname, changeMaterialPropery }) => {
  const onChangeMaterialPropery = e => {
    changeMaterialPropery(e);
  };

  const e = fillMaterialParams(entity.jsonRet.mKey, entity.jsonRet.values);

  return (
    <div key={e.key} className={cname}>
      <div className="materialPropertyLabel-a">Color</div>
      <div className="baseColorPicker-a">
        <input
          type="color"
          id={e.key}
          name="diffuseColor-hexcolor"
          defaultValue={e.baseColor}
          onChange={eb => onChangeMaterialPropery(eb)}
        />
      </div>

      <div className="materialPropertyLabel-a">Metallic</div>
      <div className="metallicSlider-a">
        <input
          type="range"
          className="slider"
          min="1"
          max="100"
          defaultValue={e.metallicValue * 100}
          id={e.key}
          name="metallicV-float100"
          onChange={eb => onChangeMaterialPropery(eb)}
        />
      </div>

      <div className="materialPropertyLabel-a">Roughness</div>
      <div className="roughnessSlider-a">
        <input
          type="range"
          className="slider"
          min="1"
          max="100"
          defaultValue={e.roughnessValue * 100}
          name="roughnessV-float100"
          id={e.key}
          onChange={eb => onChangeMaterialPropery(eb)}
        />
      </div>

      <div className="materialPropertyLabel-a">Ambient Occlusion</div>
      <div className="aoSlider-a">
        <input
          type="range"
          className="slider"
          min="1"
          max="100"
          defaultValue={e.aoValue * 100}
          name="aoV-float100"
          id={e.key}
          onChange={eb => onChangeMaterialPropery(eb)}
        />
      </div>

      <div className="materialPropertyLabel-a">Opacity</div>
      <div className="opacitySlider-a">
        <input
          type="range"
          className="slider"
          min="1"
          max="100"
          defaultValue={e.opacityValue * 100}
          name="opacity-float100"
          id={e.key}
          onChange={eb => onChangeMaterialPropery(eb)}
        />
      </div>
    </div>
  );
};

MaterialParametersEditor.propTypes = {
  currentEntity: PropTypes.object,
  changeMaterialPropery: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  { changeMaterialPropery }
)(MaterialParametersEditor);
