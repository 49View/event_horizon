import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { showConfirmAlert } from "../../../actions/confirmalert";
// import ConfirmAlert from "../ConfirmAlert";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";

const EntityMetaSection = ({ showConfirmAlert }) => {
  const onDeleteEntity = e => {
    showConfirmAlert("Confirm deletion of ", "danger");
  };

  return (
    <div className="metadata_controls-a">
      <EntityTags />
      <EntityInfo />
      {/* <ConfirmAlert /> */}
      <div className="entity-control-a">
        <EntityUpdateContent />
        <div className="my-3"></div>
        <div className="deleteentity-a">
          <input
            type="button"
            className="btn2 btn-danger"
            value="Delete :'("
            onClick={e => onDeleteEntity(e)}
          />
        </div>
      </div>
    </div>
  );
};

EntityMetaSection.propTypes = {
  showConfirmAlert: PropTypes.func.isRequired
};

export default connect(
  null,
  { showConfirmAlert }
)(EntityMetaSection);
