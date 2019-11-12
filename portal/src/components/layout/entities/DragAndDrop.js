import React, { useCallback } from "react";
import { useDispatch } from "react-redux";
import { useDropzone } from "react-dropzone";
import { changeMaterialPropery, updateTextureParameterOnMaterial } from "actions/entities";
import { checkFileExtensionsOnEntityGroup, GroupImage } from "utils/utils";
import {SET_MODAL_SELECTED_ENTITY_NAME} from "actions/types";

const DragAndDrop = props => {
  const dispatch = useDispatch();

  const onDrop = useCallback(
    acceptedFiles => {
      if (checkFileExtensionsOnEntityGroup(GroupImage, acceptedFiles[0].name)) {
        const reader = new FileReader();

        reader.onabort = () => console.log("file reading was aborted");
        reader.onerror = () => console.log("file reading has failed");
        reader.onload = () => {
          console.log("File read:", acceptedFiles[0].name);
          //dispatch(addEntity(acceptedFiles[0].name, reader.result, GroupImage));
          dispatch(changeMaterialPropery(props.entry.inputTextureName+"-string", props.entry.key_id, "49viewlogo"));
        };

        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      }
    },
    [dispatch, props]
  );
  const { getRootProps } = useDropzone({ onDrop });

  const updateMaterialTextureCallback = () => {
    dispatch({
      type: SET_MODAL_SELECTED_ENTITY_NAME,
      payload: {
        group:GroupImage,
        fatherEntityName: props.entry.key_id,
        onClickCallback: updateTextureParameterOnMaterial,
        selectedModalEntityName: props.entry.inputTextureName
      }
    });
  };

  return (
    <div {...getRootProps()} className="materialPropertyTexture" onClick={updateMaterialTextureCallback}>
      <img src={props.entry.textureName} alt="" />
    </div>
  );
};

export default DragAndDrop;
