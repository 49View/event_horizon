import React, { useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import {
  updateEntriesPartialSearch,
  replaceMaterial,
  getAllMaterialsMeta
} from "../../../actions/entities";
import EntityDragAndImport from "./EntityDragAndImport";
import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";

const SmallEntriesDialog = ({
  loading,
  entries,
  project,
  getAllMaterialsMeta
}) => {
  const [count, setCount] = useState(0);

  useEffect(() => {
    if (count === 0) {
      getAllMaterialsMeta(project);
      setCount(1);
    }
  }, [count, entries, getAllMaterialsMeta, project]);

  return loading ? (
    <Spinner />
  ) : (
    <div className="nodeViewer-a">
      <EntitiesSearchBox updatePartialSearch={updateEntriesPartialSearch} />
      <EntityDragAndImport />
      <EntitiesThumbHandler entries={entries} onClicked={replaceMaterial} />
    </div>
  );
};

SmallEntriesDialog.propTypes = {
  loading: PropTypes.bool,
  project: PropTypes.string,
  entries: PropTypes.array
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  project: state.auth.userdata.project,
  entries: state.entities.matEntriesFiltered
});

export default connect(
  mapStateToProps,
  { getAllMaterialsMeta }
)(SmallEntriesDialog);
