import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
const moment = require("moment");

const EntityInfo = ({ currentEntity }) => {
  const creationDate =
    currentEntity && currentEntity.entity.metadata.creationDate
      ? moment(currentEntity.entity.metadata.creationDate).fromNow()
      : "1st Jan 1970";
  const updateDate =
    currentEntity &&
    currentEntity.entity.metadata.lastUpdatedDate &&
    currentEntity.entity.metadata.lastUpdatedDate !==
      currentEntity.entity.metadata.creationDate
      ? moment(currentEntity.entity.metadata.lastUpdatedDate).fromNow()
      : "Never";

  return (
    <div className="entity-info-a">
      <p>
        <i className="fas fa-info-circle"> </i> Info
      </p>
      <div className="entity-info-c">
        <span
          className="metaInfiTitle small text-pale"
          style={{ minWidth: "300px" }}
        >
          <i className="fas fa-user" />
          &nbsp;owner&nbsp;
        </span>
        <span className="metaInfoValue normal text-secondary">
          {currentEntity.entity.metadata.creator
            ? currentEntity.entity.metadata.creator.name
            : "Unknown"}
        </span>
        <br />
        <span
          className="metaInfiTitle small text-pale"
          style={{ minWidth: "300px" }}
        >
          <i className="fas fa-clock" />
          &nbsp;created&nbsp;
        </span>
        <span className="metaInfoValue normal text-secondary">
          {creationDate}
        </span>
        <br />
        <span className="metaInfiTitle small text-pale">
          <i className="fas fa-calendar" />
          &nbsp;updated&nbsp;
        </span>
        <span className="metaInfoValue normal text-secondary">
          {updateDate}
        </span>
        <br />
        <span className="metaInfiTitle small text-pale">
          <i className="fas fa-code" />
          &nbsp;hash
        </span>
        <span className="metaInfoValueNoOff extra-small text-secondary">
          {currentEntity.entity.metadata.hash}
        </span>
        <br />
      </div>
    </div>
  );
};

EntityInfo.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(mapStateToProps)(EntityInfo);
