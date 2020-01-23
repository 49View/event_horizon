import React from "react";

export const GroupApp = "app";
export const GroupGeom = "geom";
export const GroupMaterial = "material";
export const GroupImage = "image";
export const GroupProfile = "profile";
export const GroupScript = "script";
export const GroupFont = "font";
export const GroupUI = "ui";

export const alphaBool = flag => {
    return flag === true ? "true" : "false";
};

export const getFileName = pathname => {
    const firstSplit = pathname.split("\\");
    if (firstSplit.length) {
        const fname = firstSplit.pop();
        const secondSplit = fname.split("/");
        if (secondSplit.length) {
            return secondSplit.pop();
        }
    }

    return pathname;
};

export const getFileNameOnlyNoExt = pathname => {
    let ret = getFileName(pathname);
    return ret.substring(0, ret.lastIndexOf(".")) || ret;
};

export const getFileNameExt = filename => {
    return filename.split('.').pop().toLowerCase();
};


export const checkCommonFileExtension = (group, ext) => {
    if (group === GroupGeom) {
        if (ext === "zip" || ext === "glb" || ext === "fbx") return true;
    } else if (group === GroupMaterial) {
        if (ext === "zip" || ext === "sbsar") return true;
    } else if (group === GroupImage) {
        if (
            ext === "jpeg" ||
            ext === "png" ||
            ext === "jpg" ||
            ext === "hdr" ||
            ext === "exr" ||
            ext === "tga" ||
            ext === "tiff" ||
            ext === "gif"
        ) {
            return true;
        }
    } else if (group === GroupFont) {
        if (ext === "ttf") return true;
    } else if (group === GroupUI) {
        if (ext === "json") return true;
    } else if (group === GroupProfile) {
        if (ext === "svg") return true;
    }

    return false;
};

export const groupHasCreateEditor = group => {
    if (group !== "" && (group === GroupUI || group === GroupScript)) {
        return true;
    }
    return false;
};

export const groupHasImportFacility = group => {
    if (group !== "" && group !== GroupScript) {
        return true;
    }
    return false;
};

export const groupHasUpdateFacility = (currEntity, group) => {
    if (currEntity && group !== GroupScript) {
        return true;
    }
    return false;
};

export const groupHasMetadataSection = (currEntity, group) => {
    if (currEntity && group !== GroupScript) {
        return true;
    }
    return false;
};

export const groupHasRenderToolbar = (currEntity, group) => {
    if (currEntity && (group === GroupGeom || group === GroupMaterial)) {
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
    if (entry.group === GroupScript) {
        if (entry.metadata.thumb === "")
            return (
                <span className="geomThumbNotFound">
          <i className="fas fa-rocket"/>
        </span>
            );
    } else if (entry.group === GroupGeom) {
        if (entry.thumb === "")
            return (
                <span className="geomThumbNotFound">
          <i className="fas fa-cubes"/>
        </span>
            );
    } else if (entry.group === GroupImage || entry.group === GroupMaterial || entry.group === GroupProfile) {
        if (entry.thumb === "") {
            return (
                <span className="imageThumbNotFound">
          <i className="far fa-frown"/>
        </span>
            );
        }
    } else if (entry.group === GroupFont) {
        return (
            <span className="imageThumbNotFound">
        <i className="fas fa-font"/>
      </span>
        );
    } else if (entry.group === GroupUI) {
        return (
            <span className="imageThumbNotFound">
        <i className="fas fa-bars"/>
      </span>
        );
    }

    return <img className="imgGreyOutline" src={entry.thumb} alt=""/>;
};
