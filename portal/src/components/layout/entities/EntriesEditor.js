import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";

const EntriesEditor = ({ entry, loading }) => {
  return loading ? (
    <Spinner />
  ) : (
    <Fragment>
      <div className="entryEditor" />
    </Fragment>
  );
};

EntriesEditor.propTypes = {
  entry: PropTypes.object,
  loading: PropTypes.bool
};

const mapStateToProps = state => ({
  entry: state.entities.entry,
  loading: state.auth.loading
});

export default connect(
  mapStateToProps,
  {}
)(EntriesEditor);
