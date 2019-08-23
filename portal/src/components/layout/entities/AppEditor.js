import React from "react";
// import PropTypes from "prop-types";
import { connect } from "react-redux";
import { UnControlled as CodeMirror } from "react-codemirror2";
// import { wscSend } from "../../../utils/webSocketClient";

require("codemirror/lib/codemirror.css");
require("codemirror/theme/material.css");
require("codemirror/theme/neat.css");
require("codemirror/mode/lua/lua.js");

const AppEditor = () => {
  return (
    <div className="appdataquad">
      <CodeMirror
        value=""
        className="react-codemirror2 appdataquad"
        options={{
          mode: "lua",
          theme: "material",
          lineNumbers: true
        }}
        onKeyPress={(editor, event) => {
          if (event.key === "Enter" && event.ctrlKey === true) {
            window.Module.addScriptLine(editor.getValue());
          }
        }}
      />
    </div>
  );
};

AppEditor.propTypes = {};

const mapStateToProps = state => ({});

export default connect(
  mapStateToProps,
  {}
)(AppEditor);
