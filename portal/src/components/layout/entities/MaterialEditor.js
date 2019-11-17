import React, { Fragment } from "react";
import MaterialParametersEditor from "./MaterialParametersEditor";
import {fillMaterialParams} from "../../../utils/materialUtils";
import {useSelector} from "react-redux";

const MaterialEditor = () => {
  let mainContent = "";

  const currentEntity = useSelector(state => state.entities.currentEntity);

  // This is a PBR material editor, make sure material is PBR, if it's not, skip it!
  if ( currentEntity.jsonRet && currentEntity.jsonRet.values.mType === "PN_SH") {
    const mat = currentEntity.jsonRet;

    let depPBRMap = [];

    // eslint-disable-next-line
    for (const dep of mat.values.mStrings) {
      depPBRMap.push( { key: dep.key, value: currentEntity.deps[dep.value] } );
    }

    const m = fillMaterialParams(mat.key, mat.values, depPBRMap);
    mainContent = (
      <div className="nodeViewer-a">
        <MaterialParametersEditor
          entity={m}
        ></MaterialParametersEditor>
      </div>
    );
  }

  return <Fragment>{mainContent}</Fragment>;
};

export default MaterialEditor;
