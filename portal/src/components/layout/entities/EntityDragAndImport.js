import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";
import axios from "axios";

import { addEntity } from "../../../actions/entities";
import { checkFileExtensionsOnEntityGroup } from "../../../utils/utils";

const EntityDragAndImport = ({ addEntity, group, user }) => {
  const alert = useAlert();
  const onDrop = useCallback(
    acceptedFiles => {
      const uploadFileToElaborate = async (fileName, fileData) => {
        const urlEnc = encodeURIComponent("elaborate/geom/" + fileName);
        console.log("Url encoded resource: ", urlEnc);
        await axios.post("fs/entity_to_elaborate/" + urlEnc, fileData, {
          headers: {
            "Content-Type": "application/octet-stream"
          }
        });
      };

      // check file dragged has a valid extension for asset type
      if (checkFileExtensionsOnEntityGroup(group, acceptedFiles[0].name)) {
        const reader = new FileReader();
        reader.onabort = () => console.log("file reading was aborted");
        reader.onerror = () => console.log("file reading has failed");
        reader.onload = e => {
          // const tags = acceptedFiles[0].name.split(/[\s,._]+/);
          // const entry = {
          //   group: group,
          //   isPublic: false,
          //   isRestricted: false,
          //   creator: {
          //     name: user ? user.name : "n/a",
          //     email: user ? user.email : "N/A"
          //   },
          //   name: acceptedFiles[0].name,
          //   thumb: "",
          //   tags: tags,
          //   deps: [],
          //   raw: encode(reader.result)
          // };
          uploadFileToElaborate(acceptedFiles[0].name, reader.result);
        };
        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      } else {
        alert.show("Wrong file type", { type: "error" });
      }
    },
    [group, alert]
  );
  const { getRootProps, getInputProps } = useDropzone({ onDrop });

  return (
    <Fragment>
      <div {...getRootProps({ className: "dropzone" })}>
        <input {...getInputProps()} />
        <p>
          <i className="fas fa-plus-circle" /> &nbsp; Add new {group}
        </p>
      </div>
    </Fragment>
  );
};

EntityDragAndImport.propTypes = {
  group: PropTypes.string,
  user: PropTypes.object,
  addEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  group: state.entities.group,
  user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
  mapStateToProps,
  { addEntity }
)(EntityDragAndImport);
