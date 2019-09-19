import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const FontEditor = ({ currentEntity }) => {
  return <Fragment></Fragment>;
};

FontEditor.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  {}
)(FontEditor);
