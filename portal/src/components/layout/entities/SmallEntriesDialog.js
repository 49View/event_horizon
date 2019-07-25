import React, { useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import {
  updateReplaceMaterialPartialSearch,
  replaceMaterial,
  getAllMaterialsMeta
} from "../../../actions/entities";
import { CLOSE_REPLACE_MATERIAL } from "../../../actions/types";

import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import store from "../../../store";

const SmallEntriesDialog = ({
  loading,
  entries,
  project,
  selectedMatName,
  getAllMaterialsMeta
}) => {
  const [count, setCount] = useState(0);

  useEffect(() => {
    if (count === 0) {
      getAllMaterialsMeta(project);
      setCount(1);
    }
  }, [count, entries, getAllMaterialsMeta, project]);

  const closeReplaceMaterial = flag => () => {
    store.dispatch({ type: CLOSE_REPLACE_MATERIAL, payload: flag });
  };

  return loading ? (
    <Spinner />
  ) : (
    <div className="nodeViewer-a">
      <div className="closeButton">
        <span className="leftFloat text-secondary normal">
          {selectedMatName}
        </span>
        <span
          className="text-danger rightFloat medium clickable"
          onClick={closeReplaceMaterial(false)}
        >
          <i className="far fa-times-circle" />
        </span>
      </div>
      <EntitiesSearchBox
        updatePartialSearch={updateReplaceMaterialPartialSearch}
        placeHolderText="Replace with..."
        extraClassName="search-bar-smaller"
      />
      <EntitiesThumbHandler entries={entries} onClicked={replaceMaterial} />
    </div>
  );
};

SmallEntriesDialog.propTypes = {
  loading: PropTypes.bool,
  project: PropTypes.string,
  selectedMatName: PropTypes.string,
  entries: PropTypes.array
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  project: state.auth.userdata.project,
  entries: state.entities.matEntriesFiltered,
  selectedMatName: state.entities.selectedMatName
});

export default connect(
  mapStateToProps,
  { getAllMaterialsMeta }
)(SmallEntriesDialog);
