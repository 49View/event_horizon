import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const EntityCreator = ({ currentEntity }) => {
  return (
    <div className="entity-creator-a">
      <p>
        <i className="fas fa-user"> </i> Creator
      </p>
      <p className="react-tags">
        {currentEntity.entity.metadata.creator
          ? currentEntity.entity.metadata.creator.name
          : "Unknown"}
      </p>
    </div>
  );
};

EntityCreator.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(mapStateToProps)(EntityCreator);
