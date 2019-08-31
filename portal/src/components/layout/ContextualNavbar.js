import React, { Fragment, useEffect, useState } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import EntityTypeTaskbar from "./entities/EntityTypeTaskbar";

const ContextualNavBar = ({ isAuth, userdata }) => {
  const [taskBarSelector, setTaskBarSelector] = useState(0);

  useEffect(() => {
    if (!isAuth) {
      setTaskBarSelector(0);
      return;
    }
    if (userdata === null) {
      setTaskBarSelector(1);
      return;
    }
    if (
      isAuth === true &&
      userdata !== null &&
      userdata.project !== "" &&
      userdata.project !== null
    ) {
      setTaskBarSelector(2);
      return;
    }
    setTaskBarSelector(0);
  }, [isAuth, userdata]);

  if (taskBarSelector === 0) {
    return <Fragment />;
  }
  if (taskBarSelector === 1) {
    return <Fragment />;
  }
  return <EntityTypeTaskbar />;
};

ContextualNavBar.propTypes = {
  isAuth: PropTypes.bool,
  userdata: PropTypes.object
};

const mapStateToProps = state => ({
  isAuth: state.auth.isAuthenticated,
  userdata: state.auth.userdata
});

export default connect(
  mapStateToProps,
  {}
)(ContextualNavBar);
