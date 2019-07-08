import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { useDropzone } from "react-dropzone";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import { addTagsToEntity } from "../../../actions/entities";
const moment = require("moment");
const ReactTags = require("react-tag-autocomplete");

const EntriesEditor = ({
  currentEntity,
  tags,
  loading,
  showConfirmAlert,
  addTagsToEntity
}) => {
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

  const handleDelete = i => {
    const ntags = tags.slice(0);
    ntags.splice(i, 1);
    let newTags = [];
    for (const tag of ntags) {
      newTags.push(tag.name);
    }
    addTagsToEntity(currentEntity.entity._id, newTags);
    // setTags({ ntags });
  };

  const handleAddition = tag => {
    let ntags = [];
    for (const tag of tags) {
      ntags.push(tag.name);
    }
    if (ntags.indexOf(tag.name) !== -1) return;
    ntags.push(tag.name);
    addTagsToEntity(currentEntity.entity._id, ntags);
  };

  const refreshTagsFromEntityName = e => {
    e.preventDefault();
    const ntags = currentEntity.entity.metadata.name.split(/[\s,._]+/);
    addTagsToEntity(currentEntity.entity._id, ntags);
  };

  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
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
            <a onClick={e => refreshTagsFromEntityName(e)} href="#!">
              <span className="rightFloat">
                <i className="fas fa-redo" />
              </span>
            </a>
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
        <ConfirmAlert />
        <div className="metaHash-a normal">Hash:</div>
        <div className="metaValueHash-a normal text-pale">
          {currentEntity.entity.metadata.hash}
        </div>
        <div className="deleteentity-a">
          <input
            type="button"
            className="btn2 btn-danger"
            value="Delete"
            onClick={e => onDeleteEntity(e)}
          />
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
  tags: PropTypes.array,
  loading: PropTypes.bool,
  showConfirmAlert: PropTypes.func.isRequired,
  addTagsToEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  tags: state.entities.currentTags ? state.entities.currentTags : [],
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  { showConfirmAlert, addTagsToEntity }
)(EntriesEditor);
