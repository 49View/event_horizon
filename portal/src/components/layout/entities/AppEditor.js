import React, { Fragment } from "react";
// import PropTypes from "prop-types";
import { connect } from "react-redux";

const AppEditor = () => {
  return (
    <Fragment>
      <div>Ciao mamma</div>
    </Fragment>
  );
};

AppEditor.propTypes = {};

const mapStateToProps = state => ({});

export default connect(
  mapStateToProps,
  {}
)(AppEditor);
