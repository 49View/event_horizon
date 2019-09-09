import React from "react";

export const alphaBool = flag => {
  return flag === true ? "true" : "false";
};

export const getFileName = pathname => {
  return pathname
    .split("\\")
    .pop()
    .split("/")
    .pop();
};

export const getFileNameOnlyNoExt = pathname => {
  let ret = getFileName(pathname);
  return ret.substring(0, ret.lastIndexOf(".")) || ret;
};

export const checkCommonFileExtension = (group, ext) => {
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
  } else if (group === "font") {
    if (ext === "ttf") return true;
  } else if (group === "ui") {
    if (ext === "json") return true;
  }

  return false;
};

export const groupHasCreateEditor = group => {
  if (group === "ui") {
    return true;
  }
  return false;
};

export const groupHasImportFacility = group => {
  if (group !== "app") {
    return true;
  }
  return false;
};

export const groupHasUpdateFacility = (currEntity, group) => {
  if (currEntity && group !== "app") {
    return true;
  }
  return false;
};

export const groupHasMetadataSection = (currEntity, group) => {
  if (currEntity && group !== "app") {
    return true;
  }
  return false;
};

export const groupHasRenderToolbar = (currEntity, group) => {
  if (currEntity && (group === "geom" || group === "material")) {
    return true;
  }
  return false;
};

export const checkFileExtensionsOnEntityGroup = (group, filename) => {
  const ext = filename
    .split(".")
    .pop()
    .toLowerCase();

  return checkCommonFileExtension(group, ext);
};

export const entityTypeSelector = entry => {
  if (entry.group === "app") {
    if (entry.metadata.thumb === "")
      return (
        <span className="geomThumbNotFound">
          <i className="fas fa-rocket" />
        </span>
      );
  } else if (entry.group === "geom") {
    if (entry.metadata.thumb === "")
      return (
        <span className="geomThumbNotFound">
          <i className="fas fa-cubes" />
        </span>
      );
  } else if (entry.group === "image" || entry.group === "material") {
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

  return <img src={entry.metadata.thumb} alt="" />;
};
