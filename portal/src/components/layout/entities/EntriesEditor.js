import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { useDropzone } from "react-dropzone";
const ReactTags = require("react-tag-autocomplete");

const EntriesEditor = ({ currentEntity, loading }) => {
  // const [tags, setTags] = useState({});

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

  const tags = [];
  if (currentEntity !== null) {
    let c = 0;
    for (const tag of currentEntity.entity.tags) {
      tags.push({ id: c, name: tag });
      c++;
    }
  }

  const handleDelete = i => {
    // const ntags = tags.tags.slice(0);
    // ntags.splice(i, 1);
    // setTags({ ntags });
  };

  const handleAddition = tag => {
    // const ntags = [].concat(tags.tags, tag);
    // setTags({ ntags });
  };

  const entityRender =
    currentEntity === null ? (
      <div className="EntryEditorRender" />
    ) : (
      <div className="EntryEditorRenderGrid">
        <div className="EntryEditorRender">
          <img className="noborders" src={currentEntity.blobURL} alt="" />
        </div>
        <div className="entity-drag-a">
          <div {...getRootProps({ className: "dropzone" })}>
            <input {...getInputProps()} />
            <p>
              <i className="fas fa-upload" /> &nbsp; Replace Asset Content
            </p>
          </div>
        </div>
        <div className="entity-tags-a">
          <p>
            <i className="fas fa-tags"> </i>Tags:
          </p>
          <ReactTags
            tags={tags}
            // suggestions={tags.suggestions}
            handleDelete={handleDelete}
            handleAddition={handleAddition}
          />
        </div>
        <div className="metaName-a normal">Name:</div>
        <div className="metaValue-a medium text-primary">
          {currentEntity.entity.name}
        </div>
        <div />
        <div className="metaHash-a normal">Hash:</div>
        <div className="metaValueHash-a normal text-pale">
          {currentEntity.entity.hash}
        </div>
        <div />
      </div>
    );

  return loading ? (
    <Spinner />
  ) : (
    <Fragment>
      <div className="editor-a entryEditor">
        {entityRender}
        {/* <div id="viewport">
          <canvas width="960" height="500" />
        </div> */}
      </div>
    </Fragment>
  );
};

EntriesEditor.propTypes = {
  currentEntity: PropTypes.object,
  loading: PropTypes.bool
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  {}
)(EntriesEditor);
