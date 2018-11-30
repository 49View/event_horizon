import React, {Component} from 'react';

import classes from './Render.css';

window.Module = {};
window.wasmBinary = null;

class Render extends Component {

    state = {
        isVisible: true,
    }

    canvas = null;

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