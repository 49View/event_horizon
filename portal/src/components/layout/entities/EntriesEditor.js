import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityCreator from "./EntityCreator";

const moment = require("moment");

const EntriesEditor = ({ currentEntity, loading, showConfirmAlert }) => {
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
        <EntityUpdateContent />
        <EntityTags />
        <EntityCreator />
        <div className="entity-dates-a">
          <p>
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
        <div />
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
  loading: PropTypes.bool,
  showConfirmAlert: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  { showConfirmAlert }
)(EntriesEditor);
