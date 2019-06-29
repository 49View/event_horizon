import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { decode } from "base64-arraybuffer";

const Entries = ({ entries, loading }) => {
  // useEffect(() => {
  //   getEntriesDaily();
  // });

  let entitiesRes = [];
  if (entries && entries.length > 0) {
    entries.map(e => {
      const b64 = decode(e.metadata.thumb);
      const bb = new Blob([b64]);
      entitiesRes.push({
        id: e._id,
        name: e.metadata.name,
        group: e.group,
        thumb: e.metadata.thumb !== "" ? URL.createObjectURL(bb) : ""
      });
      return 0;
    });
  }

  const entityTypeSelector = entry => {
    if (entry.group === "geom") {
      if (entry.thumb === "")
        return (
          <span className="geomThumbNotFound">
            <i className="fas fa-cubes" />
          </span>
        );
    } else if (entry.group === "image") {
      if (entry.thumb === "") {
        return (
          <span className="imageThumbNotFound">
            <i className="far fa-frown" />
          </span>
        );
      }
    } else if (entry.group === "font") {
      return (
        <span className="imageThumbNotFound">
          <i className="fas fa-font" />
        </span>
      );
    } else if (entry.group === "profile") {
      if (entry.thumb === "") {
        return (
          <span className="imageThumbNotFound">
            <i className="far fa-frown" />
          </span>
        );
      } else {
        return <svg>{entry.thumb}</svg>;
      }
    }

    return <img width="64" height="64" src={entry.thumb} alt="" />;
  };

  const viewMore = group => () => {};

  const thumbEntry = (
    <Fragment>
      {entitiesRes.map(entry => (
        <div className="EntityThumbnail" key={entry.id}>
          <a onClick={viewMore(entry.id)} href="#!">
            <div className="EntityThumbnailInset">
              {entityTypeSelector(entry)}
            </div>
            <div className="EntityThumbnailText">{entry.name}</div>
          </a>
        </div>
      ))}
    </Fragment>
  );

  return loading ? (
    <Spinner />
  ) : (
    <Fragment>
      <div className="thumbsEntityArea">{thumbEntry}</div>
    </Fragment>
  );
};

Entries.propTypes = {
  entries: PropTypes.array,
  loading: PropTypes.bool
};

const mapStateToProps = state => ({
  entries: state.entities.entriesFiltered,
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  {}
)(Entries);
