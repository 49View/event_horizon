import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { deleteEntity } from "../../actions/entities";
import { removeConfirmAlert } from "../../actions/confirmalert";

const ConfirmAlert = ({
  confirmalert,
  entity,
  deleteEntity,
  removeConfirmAlert
}) => {
  if (!confirmalert.alert) {
    return <div> </div>;
  }

  const onConfirmDelete = e => {
    e.preventDefault();
    deleteEntity(entity._id);
  };
  const onConfirmLeaveAsItIs = e => {
    e.preventDefault();
    removeConfirmAlert();
  };

  return (
    <div className="confirm-alert">
      <input
        type="button"
        className="btnConfirm btn-danger"
        value="Yes, delete"
        onClick={e => onConfirmDelete(e)}
      />
      <input
        type="button"
        className="btnConfirm btn-success"
        value="No, let it live"
        onClick={e => onConfirmLeaveAsItIs(e)}
      />
    </div>
  );
};

ConfirmAlert.propTypes = {
  confirmalert: PropTypes.object,
  entity: PropTypes.object,
  deleteEntity: PropTypes.func.isRequired,
  removeConfirmAlert: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  confirmalert: state.confirmalert,
  entity:
    state.entities.currentEntity !== null
      ? state.entities.currentEntity.entity
      : null
});

export default connect(
  mapStateToProps,
  { deleteEntity, removeConfirmAlert }
)(ConfirmAlert);
