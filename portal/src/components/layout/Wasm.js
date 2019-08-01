import React, { useEffect, useState } from "react";
import { connect } from "react-redux";
import PropTypes from "prop-types";
import { loadWasmComplete, setWasmLoaded } from "../../actions/wasm";

const Wasm = ({
  canvasRef,
  wasmLoaded,
  userToken,
  userSessionId,
  setWasmLoaded
}) => {
  const canvasStyle = {
    // position: "absolute",
    // display: userToken !== null && userSessionId !== null ? "inline" : "none"
    // top: "144px",
    // left: "570px",
    // width: "10px",
    // height: "10px"
  };
  const [count, setCount] = useState(0);

  useEffect(() => {
    console.log("UIDS ", userToken, "US ", userSessionId);
    if (
      userToken !== undefined &&
      userSessionId !== undefined &&
      userSessionId !== null
    ) {
      if (count === 0 && canvasRef != null) {
        loadWasmComplete("editor", canvasRef.current, userToken, userSessionId);
        // setWasmLoaded(true);
        setCount(1);
      }
    }
  }, [canvasRef, count, userToken, userSessionId]);

  return (
    <canvas
      style={canvasStyle}
      ref={canvasRef}
      className="Canvas"
      onContextMenu={e => e.preventDefault()}
    />
  );
};

Wasm.propTypes = {
  userToken: PropTypes.string,
  userSessionId: PropTypes.string,
  canvasRef: PropTypes.object,
  wasmLoaded: PropTypes.bool,
  setWasmLoaded: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userToken: state.auth.token,
  userSessionId: state.auth.userdata
    ? state.auth.session
      ? state.auth.session
      : state.auth.userdata.session
    : null,
  canvasRef: state.wasm.wasmCanvas,
  wasmLoaded: state.wasm.loaded
});

export default connect(
  mapStateToProps,
  { setWasmLoaded }
)(Wasm);
