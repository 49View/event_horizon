import React, {Fragment, useEffect} from "react";
import {useSelector, useDispatch} from "react-redux";
import {
  updateMetadataListPartialSearch,
  getMetadataListOf
} from "actions/entities";
import { CLOSE_ENTITIES_MODAL } from "actions/types";

import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import Modal from "react-bootstrap/Modal";
import Button from "react-bootstrap/Button";

const SmallEntriesDialog = () => {

  const dispatch = useDispatch();
  const metadataList = useSelector(state => state.entities.metadataList);

  console.log("SmallEntriesDialog re-render");
  useEffect(() => {
    if ( metadataList.enable ) {
      dispatch(getMetadataListOf(metadataList.group));
    }
  }, [metadataList.enable]);

  const closeModal = flag => {
    dispatch({ type: CLOSE_ENTITIES_MODAL, payload: flag });
  };

  return (
      !metadataList.enable ? <Fragment></Fragment> :
    <Modal
      show={metadataList.enable}
      onHide={() => closeModal(false)}
      size="sm"
      aria-labelledby="contained-modal-title-vcenter"
      centered
    >
      <Modal.Header closeButton>
        <Modal.Title id="contained-modal-title-vcenter">
          <span className="leftFloat text-secondary lead">
            {metadataList.sourceEntityName}
          </span>
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <EntitiesSearchBox
          updatePartialSearch={updateMetadataListPartialSearch}
          placeHolderText="Filter..."
          extraClassName="search-bar-smaller"
        />
        <EntitiesThumbHandler entries={metadataList.filtered} onClicked={metadataList.onClickCallback} />
      </Modal.Body>
      <Modal.Footer>
        <Button onClick={() => closeModal(false)}>Close</Button>
      </Modal.Footer>
    </Modal>
  );
};

export default SmallEntriesDialog;
