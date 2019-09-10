import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import {
  updateEntriesPartialSearch
  // getFullEntity
} from "../../../actions/entities";
import EntityDragAndImport from "./EntityDragAndImport";
import EntitiesSearchBox from "./EntitiesSearchBox";

import ContextualNavbar from "../ContextualNavbar";

const Entries = ({ entries, currentEntity, cname }) => {
  // console.log("CurrentEntity: ", currentEntity);

  const handler =
    currentEntity !== null && currentEntity.entity.group === "app" ? (
      <Fragment>
        <ContextualNavbar />
      </Fragment>
    ) : (
      // <AppFileHandler></AppFileHandler>
      <Fragment>
        <ContextualNavbar />
      </Fragment>
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
