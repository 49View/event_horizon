import React, { Fragment } from "react";
import { decode } from "base64-arraybuffer";
import {
  entityTypeSelector,
  checkCommonFileExtension
} from "../../../utils/utils";
import store from "../../../store";

const EntitiesThumbHandler = (props) => {

  const currentEntity = props.currentEntity;
  const entries = props.entries;
  const onClicked = props.onClicked;

  // console.log("###Current entity: ", currentEntity);
  let entitiesRes = [];
  if (entries && entries.length > 0) {
    entries.map(e => {
      let entryWithThumb = e;
      console.log("EntityThumb: ", entryWithThumb );
      if (!e.thumb.startsWith("blob:")) {
        const bb = new Blob([decode(e.thumb)]);
        entryWithThumb.thumb =
          e.thumb !== "" ? URL.createObjectURL(bb) : "";
      }
      entryWithThumb.cname = "EntityThumbnail";
      if (currentEntity && e._id === currentEntity.entity._id) {
        entryWithThumb.cname += " leftSideBarGroupSelected";
      }
      entryWithThumb.entityId = e._id;
      entitiesRes.push(entryWithThumb);
      return 0;
    });
  }

  const viewMore = entityToRender => () => {
    console.log( "Entity callback: ", entityToRender);
    store.dispatch(onClicked(entityToRender, props.callbackProps));
  };

  let displayNames = {};
  entitiesRes.forEach(entry => {
    displayNames[entry._id] = [];
    if (entry.tags.length === 0) {
      displayNames[entry._id].push(entry.name);
    } else {
      entry.tags.forEach(element => {
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
          <div className="EntityThumbnailText normal">
            {displayNames[entry._id].map(e => (
              <span key={entry._id + e}>{e} </span>
            ))}
          </div>
          <div className="EntityThumbnailOwner small">
            <span className="text-pale">
              <i className="fas fa-user-tag" />{" "}
            </span>
            <span className="text-secondary">
              {entry.creator
                ? entry.creator.name
                : entry.project}
            </span>
          </div>
        </div>
      ))}
    </Fragment>
  );
};

export default EntitiesThumbHandler;
