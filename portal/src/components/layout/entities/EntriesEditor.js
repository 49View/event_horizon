import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { useDropzone } from "react-dropzone";
const moment = require("moment");
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
    for (const tag of currentEntity.entity.metadata.tags) {
      tags.push({ id: c, name: tag });
      c++;
    }
  }

  const handleDelete = i => {
    const ntags = tags.slice(0);
    ntags.splice(i, 1);
    console.log(ntags);
    // setTags({ ntags });
  };

  const handleAddition = tag => {
    console.log(tag);
    // const ntags = [].concat(tags.tags, tag);
    // setTags({ ntags });
  };

  const creationDate =
    currentEntity && currentEntity.entity.metadata.creationDate
      ? moment(currentEntity.entity.metadata.creationDate).fromNow()
      : "1st Jan 1970";
  const updateDate =
    currentEntity &&
    currentEntity.entity.metadata.lastUpdatedDate &&
    currentEntity.entity.metadata.lastUpdatedDate !==
      currentEntity.entity.metadata.creationDate
      ? moment(currentEntity.entity.metadata.lastUpdatedDate).fromNow()
      : "Never";

  const entityRender =
    currentEntity === null ? (
      <div className="EntryEditorRender" />
    ) : (
      <div className="EntryEditorRenderGrid">
        <div className="EntryEditorRender">
          <img className="noborders" src={currentEntity.blobURL} alt="" />
        </div>
        <div className="entity-drag-a">
          <div {...getRootProps({ className: "dropzone dropzoneNoHMargins" })}>
            <input {...getInputProps()} />
            <p>
              <i className="fas fa-upload" /> &nbsp; Replace Asset Content
            </p>
          </div>
        </div>
        <div className="entity-tags-a">
          <p>
            <i className="fas fa-tags"> </i> Tags
          </p>
          <ReactTags
            tags={tags}
            // suggestions={tags.suggestions}
            handleDelete={handleDelete}
            handleAddition={handleAddition}
            allowNew={true}
          />
          <p className="entity-tags-a">
            <i className="fas fa-user"> </i> Creator
          </p>
          <p className="react-tags">
            {currentEntity.entity.metadata.creator
              ? currentEntity.entity.metadata.creator.name
              : "Unknown"}
          </p>
          <p className="entity-tags-a">
            <i className="fas fa-calendar"> </i> Dates
          </p>
          <p className="react-tags">
            <span className="small text-pale" style={{ minWidth: "300px" }}>
              created:&nbsp;
            </span>
            <span className="text-secondary">{creationDate}</span>
            <br />
            <span className="small text-pale">updated:&nbsp;</span>
            <span className="text-secondary">{updateDate}</span>
          </p>
        </div>
        <div className="metaName-a normal">Name:</div>
        <div className="metaValue-a medium text-primary">
          {currentEntity.entity.metadata.name}
        </div>
        <div />
        <div className="metaHash-a normal">Hash:</div>
        <div className="metaValueHash-a normal text-pale">
          {currentEntity.entity.metadata.hash}
        </div>
        <div className="deleteentity-a">
          <input type="submit" className="btn2 btn-danger" value="Delete" />
        </div>
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
