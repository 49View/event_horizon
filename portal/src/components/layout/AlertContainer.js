import React, {Fragment} from "react";
import Alert from 'react-bootstrap/Alert'
import {REMOVE_ALERT} from './../../actions/types'
import Button from 'react-bootstrap/Button'
import {useSelector, useDispatch} from "react-redux";

const AlertContainer = () => {

    const alerts = useSelector(state => state.alert);
    const dispatch = useDispatch();

    const dismiss = (alert) => {
        dispatch({type: REMOVE_ALERT, payload: alert.id});
    }

    if (alerts !== null && alerts.length > 0) {
        const alert = alerts[0];
        return <Alert key={alert.id} variant={alert.alertType} onClose={() => {
            dismiss(alert)
        }} dismissible>
            <Alert.Heading>{alert.alertType}</Alert.Heading>
            <p>
                {alert.msg}
            </p>
            <div className="d-flex justify-content-end">
                <Button onClick={() => {
                    dismiss(alert)
                }} variant="outline-secondary">
                    Ok, I got it.
                </Button>
            </div>
        </Alert>
    }
    return <Fragment/>
}

export default AlertContainer;
