import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";

import { addEntity } from "../../../actions/entities";
import {
  checkFileExtensionsOnEntityGroup,
  groupHasCreateEditor,
  groupHasImportFacility
} from "../../../utils/utils";

const EntityDragAndImport = ({
  addEntity,
  group,
  groupSelected,
  project,
  user
}) => {
  const alert = useAlert();
  const onDrop = useCallback(
    acceptedFiles => {
      // check file dragged has a valid extension for asset type
      if (
        checkFileExtensionsOnEntityGroup(groupSelected, acceptedFiles[0].name)
      ) {
        const reader = new FileReader();
        reader.onabort = () => console.log("file reading was aborted");
        reader.onerror = () => console.log("file reading has failed");
        reader.onload = e => {
          console.log(
            "Adding ",
            acceptedFiles[0].name,
            " as ",
            groupSelected,
            " to ",
            project
          );
          addEntity(
            acceptedFiles[0].name,
            reader.result,
            groupSelected,
            project,
            user
          );
        };
        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      } else {
        alert.show("Wrong file type", { type: "error" });
      }
    },
    [addEntity, groupSelected, project, user, alert]
  );

  const { getRootProps, getInputProps } = useDropzone({ onDrop });
  const addButton = groupHasCreateEditor(groupSelected);
  const importButton = groupHasImportFacility(groupSelected);

  return (
    <Fragment>
      {addButton && (
        <div className="leftSideBarGroupImport">
          <i className="fas fa-plus" />
        </div>
      )}
      {importButton && (
        <div className="leftSideBarGroupImport">
          <div {...getRootProps({ className: "dropzoneNoHMargins" })}>
            <input {...getInputProps()} />
            <span>
              <i className="fas fa-upload" />
            </span>
          </div>
        </div>
      )}
    </Fragment>
  );
};

EntityDragAndImport.propTypes = {
  group: PropTypes.string,
  project: PropTypes.string,
  user: PropTypes.object,
  addEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  group: state.entities.group,
  groupSelected: state.entities.groupSelected,
  project: state.auth.userdata.project,
  user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
  mapStateToProps,
  { addEntity }
)(EntityDragAndImport);
