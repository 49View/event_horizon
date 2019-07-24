import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { encode, decode } from "base64-arraybuffer";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";
import {
  getFullEntity,
  addEntity,
  updateEntriesPartialSearch
} from "../../../actions/entities";
import {
  checkFileExtensionsOnEntityGroup,
  entityTypeSelector
} from "../../../utils/utils";

const SmallEntriesDialog = ({
  entries,
  group,
  loading,
  updateEntriesPartialSearch
}) => {
  let entitiesRes = [];
  if (entries && entries.length > 0) {
    entries.map(e => {
      let entryWithThumb = e;
      if (!e.metadata.thumb.startsWith("blob:")) {
        const bb = new Blob([decode(e.metadata.thumb)]);
        entryWithThumb.metadata.thumb =
          e.metadata.thumb !== "" ? URL.createObjectURL(bb) : "";
      }
      entitiesRes.push(entryWithThumb);
      return 0;
    });
  }

  const onChange = e => {
    updateEntriesPartialSearch(e.target.value.toLowerCase());
  };

  const searchBox = (
    <Fragment>
      <div className="searchbar-a entitiesSearchBox">
        <input
          type="text"
          id="search-bar"
          placeholder="Search for..."
          onChange={e => onChange(e)}
        />
        <a href="#!" className="search-icon">
          <i className="fas fa-search" />
        </a>
      </div>
    </Fragment>
  );

  const viewMore = entityToRender => () => {
    getFullEntity(entityToRender);
  };

  const thumbEntry = (
    <Fragment>
      {searchBox}
      <div {...getRootProps({ className: "dropzone" })}>
        <input {...getInputProps()} />
        <p>
          <i className="fas fa-plus-circle" /> &nbsp; Add new {group}
        </p>
      </div>

      {entitiesRes.map(entry => (
        <div className="EntityThumbnail text-primary" key={entry._id}>
          <span onClick={viewMore(entry)}>
            <div className="EntityThumbnailInset">
              {entityTypeSelector(entry)}
            </div>
            <div className="EntityThumbnailText">{entry.metadata.name}</div>
          </span>
        </div>
      ))}
    </Fragment>
  );

  return loading ? (
    <Spinner />
  ) : (
    <Fragment>
      <div className="thumbs-a thumbsEntityArea">{thumbEntry}</div>
    </Fragment>
  );
};

SmallEntriesDialog.propTypes = {
  entries: PropTypes.array,
  loading: PropTypes.bool,
  group: PropTypes.string,
  user: PropTypes.object,
  getFullEntity: PropTypes.func.isRequired,
  updateEntriesPartialSearch: PropTypes.func.isRequired,
  addEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  entries: state.entities.entriesFiltered,
  group: state.entities.group,
  loading: state.auth.loading,
  user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
  mapStateToProps,
  { getFullEntity, addEntity, updateEntriesPartialSearch }
)(SmallEntriesDialog);
