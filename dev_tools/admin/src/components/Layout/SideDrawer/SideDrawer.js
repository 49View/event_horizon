import React from 'react'
import {NavLink} from 'react-router-dom';
import List from '@material-ui/core/List';
import Drawer from '@material-ui/core/Drawer';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';
import Typography from '@material-ui/core/Typography';
import ListSubheader from '@material-ui/core/ListSubheader';
import classes from './SideDrawer.module.css';

const sideDrawer = (props) => {

    const items = props.menuItems.map((menuItem, index) => (
        <ListItem button component={NavLink} key={'sdb-'+index} to={menuItem.link}>
            <ListItemText primary={menuItem.text} className={classes.ListItemText} />
        </ListItem>
    )) 

    let subTitle=null;
    if (props.subTitle) {
        subTitle = (
            <Typography variant="overline">
                {props.subTitle}
            </Typography>
        );
    }

    const title = (
        <ListSubheader color="default" className={classes.Title}>
            <Typography variant="h6">
                EVENT HORIZON
            </Typography>
            {subTitle}
        </ListSubheader>
    );


    return (
        <Drawer open={props.isOpen} onClose={props.onClose}>
            <div
            tabIndex={0}
            role="button"
            onClick={props.onClose}
            onKeyDown={props.onClose}
            >
                <div className={classes.ItemsContainer}>
                    <List subheader={title}>
                        {items}
                    </List>        
                </div>
            </div>
        </Drawer>
    );
}

export default sideDrawer;