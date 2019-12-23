import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Alert from "react-bootstrap/Alert";

const LocalAlert = ({ alerts }) =>
    alerts !== null &&
    alerts.length > 0 &&
    alerts.map(alert => (
        <Alert key={alert.id} variant={alert.alertType}>
            {alert.msg}
        </Alert>
    ));

LocalAlert.propTypes = {
  alerts: PropTypes.array.isRequired
};

const mapStateToProps = state => ({
  alerts: state.localalert
});

export default connect(mapStateToProps)(LocalAlert);
