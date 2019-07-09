import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";

const EntriesEditor = ({ currentEntity, loading, showConfirmAlert }) => {
  const onDeleteEntity = e => {
    // e.preventDefault();
    showConfirmAlert("Confirm deletion of ", "danger");
  };

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
        <EntityInfo />
        <div className="nameValue-a medium text-primary">
          {currentEntity.entity.metadata.name}
        </div>
        <ConfirmAlert />
        <div />
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
