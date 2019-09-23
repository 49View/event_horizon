import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { Controlled as CodeMirror } from "react-codemirror2";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";

require("codemirror/lib/codemirror.css");
require("codemirror/theme/material.css");
require("codemirror/theme/neat.css");
require("codemirror/mode/javascript/javascript");

const removeEmpty = obj => {
  Object.entries(obj).forEach(
    ([key, val]) =>
      (Array.isArray(val) && val.length === 0 && delete obj[key]) ||
      (val && typeof val === "object" && removeEmpty(val)) ||
      ((val === null || val === "") && delete obj[key])
  );
  return obj;
};

const GUIEditor = ({ currentEntityData }) => {
  const [fileC, setFileC] = useState(null);

  useEffect(() => {
    if (currentEntityData) {
      setFileC(JSON.stringify(removeEmpty(currentEntityData), null, 4));
    }
  }, [currentEntityData]);

  if (!currentEntityData) return <Fragment></Fragment>;

  const handleClick = () => {};

  return (
    <Fragment>
      <div className="nodeViewer-a">
        <CodeMirror
          value={fileC}
          className="react-codemirror2 appdataquad"
          options={{
            mode: "javascript",
            theme: "material",
            lineNumbers: true
          }}
          onBeforeChange={(editor, data, value) => {
            setFileC(value);
          }}
          onChange={(editor, data, value) => {}}
          onKeyPress={(editor, event) => {
            if (event.code === "Enter" && event.ctrlKey === true) {
              // const content = editor.getValue();
              // window.Module.addScriptLine(content);
            }
          }}
        />
      </div>
      <div className="source_controls-a">
        <ButtonGroup size="sm">
          <Button
            variant="secondary"
            value={1}
            onClick={e => {
              handleClick();
              window.Module.addScriptLine(fileC);
            }}
          >
            <i className="fas fa-play"></i>
          </Button>
        </ButtonGroup>
      </div>
    </Fragment>
  );
};

GUIEditor.propTypes = {
  currentEntityData: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntityData: state.entities.currentEntityData
});

export default connect(
  mapStateToProps,
  {}
)(GUIEditor);
