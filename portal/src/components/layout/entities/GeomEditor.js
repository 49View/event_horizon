import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { decode } from "base64-arraybuffer";
import SmallEntriesDialog from "./SmallEntriesDialog";
import { SET_SELECTED_MAT_NAME } from "../../../actions/types";
import store from "../../../store";
import { changeMaterialPropery } from "../../../actions/entities";

const zlib = require("zlib");

const rgbToHex = (r, g, b) =>
  "#" +
  [r, g, b]
    .map(x => {
      const hex = x.toString(16);
      return hex.length === 1 ? "0" + hex : hex;
    })
    .join("");

const getThumbnailURLBlobFor = (thumbsContainer, thumbName) => {
  let thumb = "";
  for (const th of thumbsContainer) {
    if (th.key === thumbName) {
      thumb = th.value;
    }
  }

  return thumb === ""
    ? "/white.png"
    : URL.createObjectURL(
        new Blob([
          new Buffer.from(zlib.inflateSync(new Buffer.from(decode(thumb))))
        ])
      );
};

const GeomEditor = ({
  currentEntityNodes,
  replaceMaterialOn,
  changeMaterialPropery
}) => {
  const onReplaceEntity = e => {
    store.dispatch({
      type: SET_SELECTED_MAT_NAME,
      payload: e.currentTarget.dataset.id
    });
  };

  const onChangeMaterialPropery = e => {
    changeMaterialPropery(e);
  };

  let matSimples = [];

  if (currentEntityNodes) {
    for (const e of currentEntityNodes.mrefs) {
      let matSimpleEntry = {
        key: e.key,
        baseColor: "#ff",
        diffuseTexture: "/white.png",
        normalTexture: "/normal.png",
        metallicTexture: "/white.png",
        roughnessTexture: "/white.png",
        aoTexture: "/white.png",
        opacityTexture: "/white.png",
        aoValue: 1.0,
        roughnessValue: 1.0,
        metallicValue: 0.0,
        opacityValue: 1.0
      };
      for (const v3f of e.value.values.mV3fs) {
        if (v3f.key === "diffuseColor") {
          const rgbC = e.value.values.mV3fs[0].value;
          matSimpleEntry.baseColor = rgbToHex(
            rgbC[0] * 255,
            rgbC[1] * 255,
            rgbC[2] * 255
          );
        }
      }
      for (const fv of e.value.values.mFloats) {
        if (fv.key === "aoV") {
          matSimpleEntry.aoValue = fv.value;
        } else if (fv.key === "roughnessV") {
          matSimpleEntry.roughnessValue = fv.value;
        } else if (fv.key === "metallicV") {
          matSimpleEntry.metallicValue = fv.value;
        } else if (fv.key === "opacityV") {
          matSimpleEntry.opacityValue = fv.value;
        }
      }
      for (const tn of e.value.values.mStrings) {
        matSimpleEntry[tn.key] = getThumbnailURLBlobFor(
          e.value.thumbValues,
          tn.key
        );
      }
      matSimples.push(matSimpleEntry);
    }
  }

  const InObjectMaterials = () => {
    return (
      <Fragment>
        <div className="nodeViewer-a">
          <div className="text-primary titleMargin">Materials</div>
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
      {replaceMaterialOn ? <SmallEntriesDialog /> : <InObjectMaterials />}
    </Fragment>
  );
};

GeomEditor.propTypes = {
  currentEntityNodes: PropTypes.object,
  replaceMaterialOn: PropTypes.bool,
  changeMaterialPropery: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntityNodes: state.entities.currentEntityNodes,
  replaceMaterialOn: state.entities.replaceMaterialOn
});

export default connect(
  mapStateToProps,
  { changeMaterialPropery }
)(GeomEditor);
