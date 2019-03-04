
import React, { Component } from 'react'
import Grid from '@material-ui/core/Grid';
import Paper from '@material-ui/core/Paper';
import classes from './Dashboard.module.css';

class Dashboard extends Component {

    render() {

        return (
            <Grid
                container
                spacing={16}
                className={classes.Demo}
                alignItems="center"
                direction="row"
                justify="center"
            >
                <Grid item>
                    <Paper
                        className={classes.Paper}
                        style={{ paddingTop: 10, paddingBottom: 10 }}
                    >
                        DASHBOARD
                    </Paper>
                </Grid>                
            </Grid>        
        )
    }
}

  
export default Dashboard;