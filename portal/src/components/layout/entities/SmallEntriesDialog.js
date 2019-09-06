import React, { useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  updateReplaceMaterialPartialSearch,
  replaceMaterial,
  getAllMaterialsMeta
} from "../../../actions/entities";
import { CLOSE_REPLACE_MATERIAL } from "../../../actions/types";

import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import store from "../../../store";
import Modal from "react-bootstrap/Modal";
import Button from "react-bootstrap/Button";

const SmallEntriesDialog = ({
  loading,
  entries,
  project,
  selectedMatName,
  smallEntityModalOn,
  getAllMaterialsMeta
}) => {
  const [count, setCount] = useState(0);

  useEffect(() => {
    if (count === 0) {
      getAllMaterialsMeta(project);
      setCount(1);
    }
  }, [count, getAllMaterialsMeta, project]);

  const closeReplaceMaterial = flag => {
    store.dispatch({ type: CLOSE_REPLACE_MATERIAL, payload: flag });
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
            {selectedMatName}
          </span>
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <EntitiesSearchBox
          updatePartialSearch={updateReplaceMaterialPartialSearch}
          placeHolderText="Filter..."
          extraClassName="search-bar-smaller"
        />
        <EntitiesThumbHandler entries={entries} onClicked={replaceMaterial} />
      </Modal.Body>
      <Modal.Footer>
        <Button onClick={() => closeReplaceMaterial(false)}>Close</Button>
      </Modal.Footer>
    </Modal>
  );
};

SmallEntriesDialog.propTypes = {
  loading: PropTypes.bool,
  smallEntityModalOn: PropTypes.bool,
  project: PropTypes.string,
  selectedMatName: PropTypes.string,
  entries: PropTypes.array
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  project: state.auth.userdata.project,
  entries: state.entities.matEntriesFiltered,
  selectedMatName: state.entities.selectedMatName,
  smallEntityModalOn: state.entities.smallEntityModalOn
});

export default connect(
  mapStateToProps,
  { getAllMaterialsMeta }
)(SmallEntriesDialog);
