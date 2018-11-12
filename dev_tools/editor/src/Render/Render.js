import React, {Component} from 'react';

import classes from './Render.css';

//WASM

window.Module = {};
window.wasmBinary = null;

class Render extends Component {

    state = {
        isVisible: true,
    }

    canvas = null;

    componentWillReceiveProps(nextProps, nextContext) {
        console.log("Receive props", nextProps);
        if (this.props.visible!==nextProps.visible) {
            if (nextProps.visible) {
                console.log("RESUME MAIN LOOP");
                this.setState({isVisible: true});
                window.Module.resumeMainLoop();
                window.Module.addScriptLine("ENABLE KEYBOARD");
            } else {
                console.log("PAUSE MAIN LOOP");
                this.setState({isVisible: false});
                // window.Module.addScriptLine("DISABLE KEYBOARD");
                // window.Module.pauseMainLoop();
            }
        }
        if (this.props.renderedPropertyId!==nextProps.renderedPropertyId) {
            if (nextProps.renderedPropertyId) {
                console.log("LOAD HOUSE "+nextProps.renderedPropertyId); 
                window.Module.addScriptLine("load house "+nextProps.renderedPropertyId);
            } else 
            {
                //CLEAN HOUSE
            }
        }
        if (this.props.searchText!==nextProps.searchText) {
            if (nextProps.searchText!==null && nextProps.searchText.trim().length>0) {
                if (this.state.isVisible) {
                    console.log("EXEC COMMAND "+nextProps.searchText); 
                    window.Module.addScriptLine(nextProps.searchText);
                    this.props.onClearSearchText();
                }
            }       
        }
        if (this.props.searchTextBoxFocused!==nextProps.searchTextBoxFocused) {
            console.log("SEARCHTEXTBOX FOCUSED: ", nextProps.searchTextBoxFocused);
            if (nextProps.searchTextBoxFocused) {
                console.log("IF RENDER IS VISIBLE DISABLE RENDER KEYBOARD"); 
                if (this.state.isVisible) {
                    console.log("DISABLE RENDER KEYBOARD"); 
                    window.Module.addScriptLine("DISABLE KEYBOARD");
                }
            } else {
                console.log("IF RENDER IS VISIBLE ENABLE RENDER KEYBOARD"); 
                if (this.state.isVisible) {
                    console.log("ENABLE RENDER KEYBOARD"); 
                    window.Module.addScriptLine("ENABLE KEYBOARD");
                }
            }     
        }
    }

    componentDidMount() {
        window.wasmLoader.instantiateWasm(this.canvas);
    }

    render() {

        const canvasClasses = [classes.Canvas];
        if (this.props.visible!==true) {
            canvasClasses.push(classes.Hidden);
        }

        return (
            <canvas ref={e => this.canvas=e } className={canvasClasses.join(' ')}>
            </canvas>
        );
    }
}

export default Render;