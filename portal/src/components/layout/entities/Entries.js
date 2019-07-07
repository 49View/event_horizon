import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { decode } from "base64-arraybuffer";
import { useDropzone } from "react-dropzone";
import { getFullEntity, addEntity } from "../../../actions/entities";

const Entries = ({ entries, getFullEntity, group, loading }) => {
  const onDrop = useCallback(acceptedFiles => {
    const reader = new FileReader();

    reader.onabort = () => console.log("file reading was aborted");
    reader.onerror = () => console.log("file reading has failed");
    reader.onload = () => {
      // Do whatever you want with the file contents
      // const binaryStr = reader.result;
    };

    acceptedFiles.forEach(file => reader.readAsBinaryString(file));
  }, []);
  const { getRootProps, getInputProps } = useDropzone({ onDrop });

  let entitiesRes = [];
  if (entries && entries.length > 0) {
    entries.map(e => {
      const b64 = decode(e.metadata.thumb);
      const bb = new Blob([b64]);
      entitiesRes.push({
        id: e._id,
        name: e.metadata.name,
        hash: e.metadata.hash,
        tags: e.metadata.tags,
        deps: e.metadata.deps,
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

  const viewMore = entityToRender => () => {
    getFullEntity(entityToRender);
  };

  const thumbEntry = (
    <Fragment>
      <div {...getRootProps({ className: "dropzone" })}>
        <input {...getInputProps()} />
        <p>
          <i className="fas fa-plus-circle" /> &nbsp; Add new {group}
        </p>
      </div>

      {entitiesRes.map(entry => (
        <div className="EntityThumbnail" key={entry.id}>
          <a onClick={viewMore(entry)} href="#!">
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
      <div className="thumbs-a thumbsEntityArea">{thumbEntry}</div>
    </Fragment>
  );
};

Entries.propTypes = {
  entries: PropTypes.array,
  loading: PropTypes.bool,
  group: PropTypes.string,
  getFullEntity: PropTypes.func.isRequired,
  addEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  entries: state.entities.entriesFiltered,
  group: state.entities.group,
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  { getFullEntity, addEntity }
)(Entries);
