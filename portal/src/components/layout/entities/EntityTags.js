import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { addTagsToEntity } from "../../../actions/entities";
const ReactTags = require("react-tag-autocomplete");

const EntityTags = ({ currentEntity, tags, addTagsToEntity }) => {
  const handleDelete = i => {
    const ntags = tags.slice(0);
    ntags.splice(i, 1);
    let newTags = [];
    // eslint-disable-next-line
    for (const tag of ntags) {
      newTags.push(tag.name);
    }
    addTagsToEntity(currentEntity.entity._id, newTags);
    // setTags({ ntags });
  };

  const handleAddition = tag => {
    let ntags = [];
    // eslint-disable-next-line
    for (const tag of tags) {
      ntags.push(tag.name);
    }
    if (ntags.indexOf(tag.name) !== -1) return;
    ntags.push(tag.name);
    addTagsToEntity(currentEntity.entity._id, ntags);
  };

  const refreshTagsFromEntityName = e => {
    e.preventDefault();
    const ntags = currentEntity.entity.metadata.name.split(/[\s,._]+/);
    addTagsToEntity(currentEntity.entity._id, ntags);
  };

  return (
    <div className="entity-tags-a">
      <i className="fas fa-tags"> </i> Tags
      <a className="px-2" onClick={e => refreshTagsFromEntityName(e)} href="#!">
        <i className="fas fa-redo" />
      </a>
      <ReactTags
        tags={tags}
        // suggestions={tags.suggestions}
        handleDelete={handleDelete}
        handleAddition={handleAddition}
        allowNew={true}
      />
    </div>
  );
};

EntityTags.propTypes = {
  currentEntity: PropTypes.object,
  tags: PropTypes.array,
  addTagsToEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  tags: state.entities.currentTags ? state.entities.currentTags : [],
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  { addTagsToEntity }
)(EntityTags);
