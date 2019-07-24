import React from "react";
import { connect } from "react-redux";
import store from "../../../store";

const EntitiesSearchBox = ({ updatePartialSearch }) => {
  return (
    <div className="searchbar-a entitiesSearchBox">
      <input
        type="text"
        id="search-bar"
        placeholder="Search for..."
        onChange={e => {
          store.dispatch(updatePartialSearch(e.target.value.toLowerCase()));
        }}
      />
    </div>
  );
};

export default connect()(EntitiesSearchBox);
