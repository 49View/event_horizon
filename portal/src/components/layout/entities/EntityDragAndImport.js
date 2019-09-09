import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";
import Button from "react-bootstrap/Button";

import { addEntity } from "../../../actions/entities";
import {
  checkFileExtensionsOnEntityGroup,
  groupHasCreateEditor,
  groupHasImportFacility
} from "../../../utils/utils";

const EntityDragAndImport = ({ addEntity, group, project, user }) => {
  const alert = useAlert();
  const onDrop = useCallback(
    acceptedFiles => {
      // check file dragged has a valid extension for asset type
      if (checkFileExtensionsOnEntityGroup(group, acceptedFiles[0].name)) {
        const reader = new FileReader();
        reader.onabort = () => console.log("file reading was aborted");
        reader.onerror = () => console.log("file reading has failed");
        reader.onload = e => {
          console.log(
            "Adding ",
            acceptedFiles[0].name,
            " as ",
            group,
            " to ",
            project
          );
          addEntity(acceptedFiles[0].name, reader.result, group, project, user);
        };
        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      } else {
        alert.show("Wrong file type", { type: "error" });
      }
    },
    [addEntity, group, project, user, alert]
  );

  const { getRootProps, getInputProps } = useDropzone({ onDrop });
  const addButton = groupHasCreateEditor(group);
  const importButton = groupHasImportFacility(group);

  const gapStyle = {
    width: "100%",
    marginBottom: "5px"
  };

  return (
    <Fragment>
      {addButton && (
        <Fragment>
          <Button variant="primary" size="sm" block>
            <i className="fas fa-plus" /> &nbsp; Add {group}
          </Button>
          <div style={gapStyle}></div>
        </Fragment>
      )}
      {importButton && (
        <div {...getRootProps({ className: "dropzone" })}>
          <input {...getInputProps()} />
          <span>
            <i className="fas fa-upload" /> &nbsp; Import {group}
          </span>
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
  project: state.auth.userdata.project,
  user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
  mapStateToProps,
  { addEntity }
)(EntityDragAndImport);
