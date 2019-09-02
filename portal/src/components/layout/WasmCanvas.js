import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";

const WasmCanvas = ({
  left,
  top,
  width,
  height,
  visible  
}) => {

  let visibility = "hidden";
  if (visible===true) {
    visibility="visible";
  }

  const canvasStyle = {
      visibility: visibility,
      width: width,
      height: height,
      left: left,
      top: top
  };      

  return (
    <canvas id="WasmCanvas" style={canvasStyle}>

    </canvas>
  );
};

const mapStateToProps = state => ({
  width: state.wasm.canvasWidth,
  height: state.wasm.canvasHeight,
  left: state.wasm.canvasLeft,
  top: state.wasm.canvasTop,
  visible: state.wasm.canvasVisible,
});

export default connect(mapStateToProps)(WasmCanvas);
