import React, { Component } from 'react'
import { connect } from 'react-redux';
import { Redirect } from 'react-router-dom';
import * as actions from '../../store/actions/index';
import Aux from '../../hoc/Auxiliary/Auxiliary';
import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogContentText from '@material-ui/core/DialogContentText';
import DialogTitle from '@material-ui/core/DialogTitle';


class Logout extends Component {

    state = {
        executed: false
    }

    onExecuteHandler = (execute) => {
        if (execute) {
            //Execute
            this.props.onLogout();
        }
        this.setState({executed:true});
    }

    render() {

        let redirect = null;
        if (this.state.executed) {
            redirect = <Redirect to="/"></Redirect>
        }

        return (
            <Aux>
                {redirect}
                <Dialog
                    disableBackdropClick
                    disableEscapeKeyDown
                    maxWidth="xs"                
                    open={!this.state.executed}
                    aria-labelledby="alert-dialog-title"
                    aria-describedby="alert-dialog-description"
                >
                    <DialogTitle id="alert-dialog-title">{"Conferma"}</DialogTitle>
                    <DialogContent>
                        <DialogContentText id="alert-dialog-description">
                            Confermi la disconnessione dal portale?
                        </DialogContentText>
                    </DialogContent>
                    <DialogActions>
                        <Button variant="outlined" color="primary" onClick={() => this.onExecuteHandler(true)}>
                            Si
                        </Button>
                        <Button variant="outlined" color="primary" onClick={() => this.onExecuteHandler(false)} autoFocus>
                            No
                        </Button>
                    </DialogActions>
                </Dialog>
            </Aux>
        )
    }
}

const mapDispatchToProps = dispatch => {
    return {
      onLogout: () => dispatch(actions.logout()),
    }
  }
  
export default connect(null,mapDispatchToProps)(Logout);
