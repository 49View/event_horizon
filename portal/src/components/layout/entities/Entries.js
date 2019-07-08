import React, { Fragment, useCallback } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import { encode, decode } from "base64-arraybuffer";
import { useDropzone } from "react-dropzone";
import { useAlert } from "react-alert";
import { getFullEntity, addEntity } from "../../../actions/entities";

export const checkFileExtensionsOnEntityGroup = (group, filename) => {
  const ext = filename
    .split(".")
    .pop()
    .toLowerCase();

  if (group === "material") {
    if (ext === "zip") return true;
  } else if (group === "image") {
    if (
      ext === "jpeg" ||
      ext === "png" ||
      ext === "jpg" ||
      ext === "exr" ||
      ext === "tga" ||
      ext === "tiff" ||
      ext === "gif"
    ) {
      return true;
    }
  }

  return false;
};

const Entries = ({
  entries,
  getFullEntity,
  addEntity,
  group,
  user,
  loading
}) => {
  const alert = useAlert();
  const onDrop = useCallback(
    acceptedFiles => {
      // check file dragged has a valid extension for asset type
      if (checkFileExtensionsOnEntityGroup(group, acceptedFiles[0].name)) {
        const reader = new FileReader();
        reader.onabort = () => console.log("file reading was aborted");
        reader.onerror = () => console.log("file reading has failed");
        reader.onload = e => {
          const tags = acceptedFiles[0].name.split(/[\s,._]+/);
          const entry = {
            group: group,
            isPublic: false,
            isRestricted: false,
            creator: {
              name: user ? user.name : "n/a",
              email: user ? user.email : "N/A"
            },
            name: acceptedFiles[0].name,
            thumb: "",
            tags: tags,
            deps: [],
            raw: encode(reader.result)
          };
          addEntity(entry);
        };
        acceptedFiles.forEach(file => reader.readAsArrayBuffer(file));
      } else {
        alert.show("Wrong file type", { type: "error" });
      }
    },
    [group, user, addEntity, alert]
  );
  const { getRootProps, getInputProps } = useDropzone({ onDrop });

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

  const entityTypeSelector = entry => {
    if (entry.group === "geom") {
      if (entry.metadata.thumb === "")
        return (
          <span className="geomThumbNotFound">
            <i className="fas fa-cubes" />
          </span>
        );
    } else if (entry.group === "image") {
      if (entry.metadata.thumb === "") {
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
      if (entry.metadata.thumb === "") {
        return (
          <span className="imageThumbNotFound">
            <i className="far fa-frown" />
          </span>
        );
      } else {
        return <svg>{entry.metadata.thumb}</svg>;
      }
    }

    return <img width="64" height="64" src={entry.metadata.thumb} alt="" />;
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
        <div className="EntityThumbnail" key={entry._id}>
          <a onClick={viewMore(entry)} href="#!">
            <div className="EntityThumbnailInset">
              {entityTypeSelector(entry)}
            </div>
            <div className="EntityThumbnailText">{entry.metadata.name}</div>
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
  user: PropTypes.object,
  getFullEntity: PropTypes.func.isRequired,
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
  { getFullEntity, addEntity }
)(Entries);
