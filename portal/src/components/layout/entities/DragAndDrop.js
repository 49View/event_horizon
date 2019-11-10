import React, { useCallback } from "react";
import { useDispatch } from "react-redux";
import { useDropzone } from "react-dropzone";
import { addEntity } from "actions/entities";
import { checkFileExtensionsOnEntityGroup, GroupImage } from "utils/utils";
import {SET_MODAL_SELECTED_ENTITY_NAME} from "../../../actions/types";

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
          dispatch(addEntity(acceptedFiles[0].name, reader.result, GroupImage));
        };

        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      }
    },
    [dispatch]
  );
  const { getRootProps } = useDropzone({ onDrop });

  const onReplaceEntity = () => {
    console.log("Dispatching");
    dispatch({
      type: SET_MODAL_SELECTED_ENTITY_NAME,
      payload: {
        group:GroupImage,
        onClickCallback: null,
        // selectedModalEntityName: e.currentTarget.dataset.id
      }
    });
  };

  return (
    <div {...getRootProps()} className="materialPropertyTexture" onClick={onReplaceEntity}>
      <img src={props.imgSrc} alt="" />
    </div>
  );
};

export default DragAndDrop;