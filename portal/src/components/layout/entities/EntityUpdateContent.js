import React, { useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { useDropzone } from "react-dropzone";

const EntityUpdateContent = ({ currentEntity }) => {
  const onDrop = useCallback(acceptedFiles => {
    const reader = new FileReader();

    reader.onabort = () => console.log("file reading was aborted");
    reader.onerror = () => console.log("file reading has failed");
    reader.onload = () => {
      // Do whatever you want with the file contents
      // const binaryStr = reader.result;
    };

    acceptedFiles.forEach(file => reader.readAsBinaryString(file));
  }, []);
  const { getRootProps, getInputProps } = useDropzone({ onDrop });

  return (
    <div className="entity-drag-a">
      <div {...getRootProps({ className: "dropzone dropzoneNoHMargins" })}>
        <input {...getInputProps()} />
        <p>
          <i className="fas fa-upload" /> &nbsp; Replace Asset Content
        </p>
      </div>
    </div>
  );
};

EntityUpdateContent.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(mapStateToProps)(EntityUpdateContent);
