import React, { useState, useEffect } from "react";
import { connect } from "react-redux";
import { loadWasmComplete } from "../../actions/wasm";
import { GroupScript, GroupMaterial, GroupGeom } from "../../utils/utils";

const WasmCanvas = ({
  left,
  top,
  width,
  height,
  visible,
  currentEntity,
  group,
  userToken,
  userData
}) => {
  let canvas = React.useRef(null);
  const [count, setCount] = useState(0);

  useEffect(() => {
    if (count === 0 && userData) {
      loadWasmComplete("editor", canvas.current, userToken, userData.session);
      setCount(1);
    }
  }, [canvas, count, userToken, userData]);

  const canvasVisibility =
    currentEntity &&
    (group === GroupScript || group === GroupGeom || group === GroupMaterial)
      ? "visible"
      : "hidden";

  const bShowEntityCanvas = currentEntity && group !== GroupScript;

  const canvasSizeXNum = width; // bShowEntityCanvas ? 529 : 510;
  const canvasSizeYNum = height; //bShowEntityCanvas ? 536 : 286;

  const canvasSizeX = canvasSizeXNum.toString() + "px";
  const canvasSizeY = canvasSizeYNum.toString() + "px";

  const canvasClientSizeX =
    (canvasSizeXNum * (window.devicePixelRatio || 1)).toString() + "px";
  const canvasClientSizeY =
    (canvasSizeYNum * (window.devicePixelRatio || 1)).toString() + "px";

  const canvasPadding = bShowEntityCanvas ? "5px" : "1px";
  const canvasMargin = bShowEntityCanvas ? "0px" : "0px";
  const canvasRadius = bShowEntityCanvas ? "10px" : "3px";

  const canvasStyle = {
    position: "absolute",
    visibility: canvasVisibility,
    width: canvasSizeX,
    height: canvasSizeY,
    left: left,
    top: top,
    margin: canvasMargin,
    padding: canvasPadding,
    borderRadius: canvasRadius
  };

  // console.log("Canvas attriubes: ", canvasStyle);
  // let visibility = "hidden";
  // if (visible === true) {
  //   visibility = "visible";
  // }
  return (
    <canvas
      id="WasmCanvas"
      width={canvasClientSizeX}
      height={canvasClientSizeY}
      style={canvasStyle}
      ref={canvas}
      className="Canvas"
      onContextMenu={e => e.preventDefault()}
    />
  );

  // return <canvas id="WasmCanvas" style={canvasStyle}></canvas>;
};

const mapStateToProps = state => ({
  width: state.wasm.canvasWidth,
  height: state.wasm.canvasHeight,
  left: state.wasm.canvasLeft,
  top: state.wasm.canvasTop,
  visible: state.wasm.canvasVisible,
  currentEntity: state.entities.currentEntity,
  group: state.entities.group,
  userToken: state.auth.token,
  userData: state.auth.userdata
});

export default connect(mapStateToProps)(WasmCanvas);
