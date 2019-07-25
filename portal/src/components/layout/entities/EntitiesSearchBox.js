import React from "react";
import { connect } from "react-redux";
import store from "../../../store";

const EntitiesSearchBox = ({
  updatePartialSearch,
  placeHolderText,
  extraClassName
}) => {
  const computedInputClassName = "search-bar " + extraClassName;
  return (
    <div className="searchbar-a entitiesSearchBox">
      <input
        className={computedInputClassName}
        type="text"
        id="search-bar"
        placeholder={placeHolderText}
        onChange={e => {
          store.dispatch(updatePartialSearch(e.target.value.toLowerCase()));
        }}
      />
    </div>
  );
};

export default connect()(EntitiesSearchBox);
