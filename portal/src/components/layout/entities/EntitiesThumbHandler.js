import React, { Fragment } from "react";
import { connect } from "react-redux";
import { decode } from "base64-arraybuffer";
import { entityTypeSelector } from "../../../utils/utils";
import store from "../../../store";

const EntitiesThumbHandler = ({ entries, onClicked }) => {
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

  const viewMore = entityToRender => () => {
    store.dispatch(onClicked(entityToRender));
  };

  return (
    <Fragment>
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
};

export default connect()(EntitiesThumbHandler);
