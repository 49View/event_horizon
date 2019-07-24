import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import {
  updateEntriesPartialSearch,
  getFullEntity
} from "../../../actions/entities";
import EntityDragAndImport from "./EntityDragAndImport";
import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";

const Entries = ({ loading, entries }) => {
  return loading ? (
    <Spinner />
  ) : (
    <div className="thumbs-a thumbsEntityArea">
      <EntitiesSearchBox updatePartialSearch={updateEntriesPartialSearch} />
      <EntityDragAndImport />
      <EntitiesThumbHandler entries={entries} onClicked={getFullEntity} />
    </div>
  );
};

Entries.propTypes = {
  loading: PropTypes.bool,
  entries: PropTypes.array
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  entries: state.entities.entriesFiltered
});

export default connect(
  mapStateToProps,
  {}
)(Entries);
