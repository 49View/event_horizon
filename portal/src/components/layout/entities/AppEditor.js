import React, { Fragment } from "react";
// import PropTypes from "prop-types";
import { connect } from "react-redux";
import { UnControlled as CodeMirror } from "react-codemirror2";
import SourceControlToolbar from "./SourceControlToolbar";
// import { wscSend } from "../../../utils/webSocketClient";
import Tabs from "react-bootstrap/Tabs";
import Tab from "react-bootstrap/Tab";

require("codemirror/lib/codemirror.css");
require("codemirror/theme/material.css");
require("codemirror/theme/neat.css");
require("codemirror/mode/lua/lua.js");

const AppEditor = () => {
  return (
    <Fragment>
      <div className="source_tabs-a">
        <Tabs defaultActiveKey="profile" id="uncontrolled-tab-example">
          <Tab eventKey="home" title="Home"></Tab>
          <Tab eventKey="profile" title="Profile"></Tab>
          <Tab eventKey="contact" title="Contact" disabled></Tab>
        </Tabs>
      </div>
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
            if (event.code === "Enter" && event.ctrlKey === true) {
              window.Module.addScriptLine(editor.getValue());
            }
          }}
        />
      </div>
      <div className="source_controls-a">
        <SourceControlToolbar></SourceControlToolbar>
      </div>
    </Fragment>
  );
};

AppEditor.propTypes = {};

const mapStateToProps = state => ({});

export default connect(
  mapStateToProps,
  {}
)(AppEditor);
