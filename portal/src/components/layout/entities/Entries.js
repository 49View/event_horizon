import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  updateEntriesPartialSearch,
  getFullEntity
} from "../../../actions/entities";
import EntityDragAndImport from "./EntityDragAndImport";
import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";
import AppFileHandler from "./AppFileHandler";

const Entries = ({ entries, currentEntity, cname }) => {
  console.log("CurrentEntity: ", currentEntity);

  const handler =
    currentEntity !== null && currentEntity.entity.group === "app" ? (
      <AppFileHandler></AppFileHandler>
    ) : (
      <EntitiesThumbHandler
        currentEntity={currentEntity}
        entries={entries}
        onClicked={getFullEntity}
      />
    );

  return (
    <div className={cname}>
      <EntitiesSearchBox
        updatePartialSearch={updateEntriesPartialSearch}
        placeHolderText="Filter..."
      />
      <EntityDragAndImport />
      {handler}
    </div>
  );
};

Entries.propTypes = {
  loading: PropTypes.bool,
  entries: PropTypes.array,
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  entries: state.entities.entriesFiltered,
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  {}
)(Entries);
