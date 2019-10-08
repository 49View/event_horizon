import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";

import { addEntity, addPlaceHolderEntity } from "../../../actions/entities";
import {
  checkFileExtensionsOnEntityGroup,
  groupHasCreateEditor,
  groupHasImportFacility
} from "../../../utils/utils";

const EntityDragAndImport = ({
  addEntity,
  addPlaceHolderEntity,
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
      <div className="leftSideBarGroupImport">
        <ButtonGroup size="sm" type="checkbox">
          {addButton && (
            <Button
              variant="secondary"
              value={1}
              onClick={e => {
                addPlaceHolderEntity(groupSelected);
              }}
            >
              <i className="fas fa-plus"></i>
            </Button>
          )}
          {importButton && (
            <Button variant="secondary" value={2}>
              <div {...getRootProps({ className: "dropzoneNoHMargins" })}>
                <input {...getInputProps()} />
                <span>
                  <i className="fas fa-upload" />
                </span>
              </div>
            </Button>
          )}
        </ButtonGroup>
      </div>
    </Fragment>
  );
};

EntityDragAndImport.propTypes = {
  group: PropTypes.string,
  project: PropTypes.string,
  user: PropTypes.object,
  addEntity: PropTypes.func.isRequired,
  addPlaceHolderEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  group: state.entities.group,
  groupSelected: state.entities.groupSelected,
  project: state.auth.userdata.project,
  user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
  mapStateToProps,
  { addEntity, addPlaceHolderEntity }
)(EntityDragAndImport);
