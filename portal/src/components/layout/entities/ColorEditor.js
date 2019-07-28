import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import EntityUpdateContent from "./EntityUpdateContent";
import EntityMetaSection from "./EntityMetaSection";

const ImageEditor = ({ currentEntity, loading }) => {
  let mainContent = <Fragment />;

  if (currentEntity) {
    mainContent = (
      <div className="EntryEditorRender">
        <img className="bigimagequad" src={currentEntity.blobURL} alt="" />
      </div>
    );
  }

  const entityRender =
    currentEntity === null ? (
      <div className="EntryEditorRender" />
    ) : (
      <div className="GeomEditorRenderGrid">
        <div className="nameValue-a medium text-primary">
          {currentEntity.entity.metadata.name}
        </div>
        <EntityUpdateContent />
        {mainContent}
        <EntityMetaSection />
      </div>
    );

  return (
    <Fragment>
      {loading && <Spinner />}
      <div className="editor-a entryEditor">{entityRender}</div>
    </Fragment>
  );
};

ImageEditor.propTypes = {
  currentEntity: PropTypes.object,
  loading: PropTypes.bool
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  loading: state.entities.loading
});

export default connect(
  mapStateToProps,
  {}
)(ImageEditor);
