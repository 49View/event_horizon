import React, { useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  updateReplaceMaterialPartialSearch,
  getMetadataListOf
} from "../../../actions/entities";
import { CLOSE_ENTITIES_MODAL } from "../../../actions/types";

import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import store from "../../../store";
import Modal from "react-bootstrap/Modal";
import Button from "react-bootstrap/Button";

const SmallEntriesDialog = ({
  group,
  entries,
  project,
  selectedModalEntityName,
  smallEntityModalOn,
  getMetadataListOf,
  onClickCallback
}) => {
  useEffect(() => {
    getMetadataListOf(group, project);
  }, [getMetadataListOf, project, group]);

  const closeReplaceMaterial = flag => {
    store.dispatch({ type: CLOSE_ENTITIES_MODAL, payload: flag });
  };

  return (
    <Modal
      show={smallEntityModalOn}
      onHide={() => closeReplaceMaterial(false)}
      size="sm"
      aria-labelledby="contained-modal-title-vcenter"
      centered
    >
      <Modal.Header closeButton>
        <Modal.Title id="contained-modal-title-vcenter">
          <span className="leftFloat text-secondary lead">
            {selectedModalEntityName}
          </span>
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <EntitiesSearchBox
          updatePartialSearch={updateReplaceMaterialPartialSearch}
          placeHolderText="Filter..."
          extraClassName="search-bar-smaller"
        />
        <EntitiesThumbHandler entries={entries} onClicked={onClickCallback} />
      </Modal.Body>
      <Modal.Footer>
        <Button onClick={() => closeReplaceMaterial(false)}>Close</Button>
      </Modal.Footer>
    </Modal>
  );
};

SmallEntriesDialog.propTypes = {
  smallEntityModalOn: PropTypes.bool,
  project: PropTypes.string,
  selectedModalEntityName: PropTypes.string,
  entries: PropTypes.array,
  group: PropTypes.string,
  onClickCallback: PropTypes.func.isRequired
};

const mapStateToProps = (state, ownProps) => ({
  project: state.auth.userdata.project,
  entries: state.entities.matEntriesFiltered,
  selectedModalEntityName: state.entities.selectedModalEntityName,
  smallEntityModalOn: state.entities.smallEntityModalOn,
  group: ownProps.group,
  onClickCallback: ownProps.onClickCallback
});

export default connect(
  mapStateToProps,
  { getMetadataListOf }
)(SmallEntriesDialog);
