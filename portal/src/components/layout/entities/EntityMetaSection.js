import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { showConfirmAlert } from "../../../actions/confirmalert";
import ConfirmAlert from "../ConfirmAlert";
import EntityTags from "./EntityTags";
import EntityInfo from "./EntityInfo";

const EntityMetaSection = ({ showConfirmAlert }) => {
  const onDeleteEntity = e => {
    showConfirmAlert("Confirm deletion of ", "danger");
  };

  return (
    <Fragment>
      <EntityTags />
      <EntityInfo />
      <ConfirmAlert />
      <div />
      <div className="deleteentity-a">
        <input
          type="button"
          className="btn2 btn-danger"
          value="Delete :'("
          onClick={e => onDeleteEntity(e)}
        />
      </div>
    </Fragment>
  );
};

EntityMetaSection.propTypes = {
  showConfirmAlert: PropTypes.func.isRequired
};

export default connect(
  null,
  { showConfirmAlert }
)(EntityMetaSection);
