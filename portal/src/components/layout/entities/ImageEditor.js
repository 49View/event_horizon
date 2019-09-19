import React, { Fragment } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const ImageEditor = ({ currentEntity }) => {
  return <Fragment></Fragment>;
};

ImageEditor.propTypes = {
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  {}
)(ImageEditor);
