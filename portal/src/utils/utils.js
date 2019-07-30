import React from "react";

export const checkFileExtensionsOnEntityGroup = (group, filename) => {
  const ext = filename
    .split(".")
    .pop()
    .toLowerCase();

  if (group === "geom") {
    if (ext === "glb" || ext === "fbx") return true;
  } else if (group === "material") {
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

export const entityTypeSelector = entry => {
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
