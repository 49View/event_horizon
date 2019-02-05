import React, {Component} from 'react';
import { connect } from 'react-redux';
import * as actions from '../../store/actions/index';

import classes from './Render.css';

class Render extends Component {

    canvas = null;

    componentDidMount() {
        this.props.onRunWasm(this.canvas);
    }

    render() {

        return (
            <canvas ref={e => this.canvas=e } className={classes.Canvas}>
            </canvas>
        );
    }
}


const mapDispatchToProps = dispatch => {
    return {
      onRunWasm: (canvas) => dispatch(actions.runWasm(canvas))
    }
}
  
  export default connect(null, mapDispatchToProps)(Render);
  