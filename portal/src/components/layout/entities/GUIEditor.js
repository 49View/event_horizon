import React, { Fragment, useState, useEffect } from "react";
import { useSelector } from "react-redux";
import { Controlled as CodeMirror } from "react-codemirror2";
import { updateAsset } from "../../../utils/webSocketClient";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";
import axios from "axios";

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

const GUIEditor = () => {
  const [fileC, setFileC] = useState(null);

  const currentEntityData = useSelector(
    state => state.entities.currentEntityData
  );
  const currentEntity = useSelector(state => state.entities.currentEntity);

  useEffect(() => {
    if (currentEntityData) {
      setFileC(JSON.stringify(removeEmpty(currentEntityData), null, 4));
    }
  }, [currentEntityData]);

  if (!currentEntityData) return <Fragment></Fragment>;

  const handleClick = fileC => {
    try {
      const jc = JSON.parse(fileC);
      updateAsset(jc, currentEntity);
    } catch (error) {
      console.log("JSON error for GUI schema: ", error);
    }
  };

  const saveEntityData = async fileC => {
    try {
      const jc = JSON.parse(fileC);
      const res = await axios.put("/api/entities/" + currentEntity.entity._id, jc, {
        headers: {
          "Content-Type": "application/json"
        }
      });
      if (res.status === 204) {
        await axios.post("/api/entities/" +
            currentEntity.entity.group +
            "/" +
            currentEntity.entity.metadata.name,
          jc,
          {
            headers: {
              "Content-Type": "application/octet-stream"
            }
          }
        );
      }
    } catch (error) {
      console.log("JSON error for GUI schema: ", error);
    }
  };

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
              handleClick(fileC);
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
              handleClick(fileC);
            }}
          >
            <i className="fas fa-play"></i>
          </Button>
          <Button
            variant="secondary"
            value={1}
            onClick={e => {
              saveEntityData(fileC);
            }}
          >
            <i className="fas fa-save"></i>
          </Button>
        </ButtonGroup>
      </div>
    </Fragment>
  );
};

export default GUIEditor;
