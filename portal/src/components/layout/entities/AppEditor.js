import React from "react";
// import PropTypes from "prop-types";
import { connect } from "react-redux";

const AppEditor = () => {
  return <div className="appdataquad">Ciao mamma</div>;
};

AppEditor.propTypes = {};

const mapStateToProps = state => ({});

export default connect(
  mapStateToProps,
  {}
)(AppEditor);
