
import React, { Component } from 'react'
import Grid from '@material-ui/core/Grid';
import classes from './Layout.module.css';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';
import AccountCircle from '@material-ui/icons/AccountCircle';
import Tooltip from '@material-ui/core/Tooltip';
import Aux from '../../hoc/Auxiliary/Auxiliary';
import SideDrawer from './SideDrawer/SideDrawer';

class Layout extends Component {

    state = {
        showSideDrawer: false,
    }

    sideDrawerToggleHandler = (open) => {
        this.setState({showSideDrawer: open});
    }
    globalMenuItems = [
        {
            text: "Login",
            link: "Login",
            guest: true
        },
        {
            text: "Logout",
            user: true,
            link: "Logout"
        }
    ]
    
    render() {

        let menuItems = [];
        let menuButton = null;
        if (this.props.isAuthenticated || this.props.isGuest) {
            menuButton = (
                <IconButton className={classes.MenuButton} color="inherit" aria-label="Menu" onClick={() => this.sideDrawerToggleHandler(true)}>
                    <MenuIcon/>
                </IconButton>
            );

            if (this.props.isAuthenticated) {
                menuItems=this.globalMenuItems.filter(mi => mi.user===true);
            } else if (this.props.isGuest) {
                menuItems=this.globalMenuItems.filter(mi => mi.guest===true);
            }           

        }
        let userName=null;
        let userArea=null;
        if (this.props.user!==null) {
            userName=this.props.user.name;
            userArea=(
                <Aux>
                    <div className={classes.Grow} />
                    <Tooltip disableFocusListener disableTouchListener title={this.props.user.name.toUpperCase()} placement="left">
                        <AccountCircle />
                    </Tooltip>                    
                </Aux>
            );
        }
        const sideDrawer = (
            <SideDrawer 
                isOpen={this.state.showSideDrawer} 
                onClose={() => this.sideDrawerToggleHandler(false)} 
                menuItems={menuItems} 
                subTitle={userName} 
                onClickLink={this.clickLinkHandler}/>
        );

        return (
            <Aux>
                <AppBar position="static">
                    <Toolbar>
                        {menuButton}
                        <Typography variant="h6" color="inherit" className={classes.Grow}>
                            EVENT HORIZON
                        </Typography>
                        {userArea}
                    </Toolbar>
                </AppBar>
                {sideDrawer}
                <Grid container className={classes.Container}>
                    <Grid item xs={12}>
                        {this.props.children}
                    </Grid>        
                </Grid>        
            </Aux>
        )
    }
}

  
export default Layout;