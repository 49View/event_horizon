import React, { Fragment, useEffect } from "react";
import { connect } from "react-redux";
import { decode } from "base64-arraybuffer";
import {
  entityTypeSelector,
  checkCommonFileExtension
} from "../../../utils/utils";
import store from "../../../store";

const EntitiesThumbHandler = ({ currentEntity, entries, onClicked }) => {
  useEffect(() => {
    // if (entries.length === 1 && !currentEntity) {
    //   store.dispatch(onClicked(entries[0]));
    // }
  }, [entries, currentEntity, onClicked]);

  // console.log("###Current entity: ", currentEntity);
  let entitiesRes = [];
  if (entries && entries.length > 0) {
    entries.map(e => {
      let entryWithThumb = e;
      if (!e.metadata.thumb.startsWith("blob:")) {
        const bb = new Blob([decode(e.metadata.thumb)]);
        entryWithThumb.metadata.thumb =
          e.metadata.thumb !== "" ? URL.createObjectURL(bb) : "";
      }
      entryWithThumb.cname = "EntityThumbnail";
      if (currentEntity && e._id === currentEntity.entity._id) {
        entryWithThumb.cname += " leftSideBarGroupSelected";
      }

      entitiesRes.push(entryWithThumb);
      return 0;
    });
  }

  const viewMore = entityToRender => () => {
    store.dispatch(onClicked(entityToRender));
  };

  let displayNames = {};
  entitiesRes.forEach(entry => {
    displayNames[entry._id] = [];
    if (entry.metadata.tags.length === 0) {
      displayNames[entry._id].push(entry.metadata.name);
    } else {
      entry.metadata.tags.forEach(element => {
        if (!checkCommonFileExtension(entry.group, element)) {
          displayNames[entry._id].push(element);
        }
      });
    }
  });

  return (
    <Fragment>
      {entitiesRes.map(entry => (
        <div className={entry.cname} key={entry._id} onClick={viewMore(entry)}>
          <div className="EntityThumbnailInset">
            {entityTypeSelector(entry)}
          </div>
          <div className="EntityThumbnailText text-primary normal">
            {displayNames[entry._id].map(e => (
              <span key={entry._id + e}>{e} </span>
            ))}
          </div>
          <div className="EntityThumbnailOwner small">
            <span className="text-pale">
              <i className="fas fa-user-tag" />{" "}
            </span>
            <span className="text-secondary">
              {entry.metadata.creator
                ? entry.metadata.creator.name
                : entry.project}
            </span>
          </div>
        </div>
      ))}
    </Fragment>
  );
};

export default connect()(EntitiesThumbHandler);
