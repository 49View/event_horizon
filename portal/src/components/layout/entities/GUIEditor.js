import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { Controlled as CodeMirror } from "react-codemirror2";
// import ButtonGroup from "react-bootstrap/ButtonGroup";
// import Button from "react-bootstrap/Button";
import { useIndexedDB } from "react-indexed-db";

require("codemirror/lib/codemirror.css");
require("codemirror/theme/material.css");
require("codemirror/theme/neat.css");
require("codemirror/mode/javascript/javascript");

const GUIEditor = ({ currentEntity, userData, wasmLogs, wasmOutputDirty }) => {
  const { getAll } = useIndexedDB("files");
  const [fileData, setFileData] = useState(null);
  const [fileC, setFileC] = useState(null);

  useEffect(() => {
    if (
      currentEntity !== null &&
      currentEntity.entity !== null &&
      fileData === null
    ) {
      getAll().then(filesFromDb => {
        filesFromDb.some(fileFromDb => {
          if (
            fileFromDb.project === userData.project &&
            fileFromDb.app === currentEntity.entity.mKey &&
            fileFromDb.file === currentEntity.entity.metadata.name &&
            fileFromDb.user === userData.user.email
          ) {
            setFileData(fileFromDb);
            setFileC(fileFromDb.content);
            return true;
          }
          return false;
        });
      });
    }
  }, [userData, currentEntity, fileData, fileC, getAll]);

  // const handleClick = () => {
  //   if (fileData === null) {
  //     // console.log("ADD NEW FILE");
  //     add({
  //       project: userData.project,
  //       app: currentEntity.entity.mKey,
  //       file: currentEntity.entity.metadata.name,
  //       user: userData.user.email,
  //       content: fileC
  //     }).then(
  //       event => {
  //         let updatedFileData = {
  //           id: event,
  //           project: userData.project,
  //           app: currentEntity.entity.mKey,
  //           file: currentEntity.entity.metadata.name,
  //           user: userData.user.email,
  //           content: fileC
  //         };
  //         setFileData(updatedFileData);
  //       },
  //       error => {
  //         console.log(error);
  //       }
  //     );
  //   } else {
  //     // console.log("UPDATE FILE");
  //     update({ ...fileData, id: fileData.id, content: fileC }).then(
  //       event => {
  //         // console.log("FILE UPDATED: ", event);
  //       },
  //       error => {
  //         console.log(error);
  //       }
  //     );
  //   }
  // };

  return (
    <Fragment>
      <div className="ui_tabs-a"></div>
      <div className="ui_source-a">
        <CodeMirror
          value={fileC}
          className="react-codemirror2 ui_source-a"
          options={{
            mode: "json",
            theme: "material",
            lineNumbers: true
          }}
          onBeforeChange={(editor, data, value) => {
            setFileC(value);
          }}
          onChange={(editor, data, value) => {}}
          onKeyPress={(editor, event) => {
            if (event.code === "Enter" && event.ctrlKey === true) {
              const content = editor.getValue();
              window.Module.addScriptLine(content);
              // handleClick();
            }
          }}
        />
      </div>
    </Fragment>
  );
};

GUIEditor.propTypes = {
  currentEntity: PropTypes.object,
  userData: PropTypes.object,
  appKey: PropTypes.string
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  userData: state.auth.userdata,
  appKey: state.entities.appKey
});

export default connect(
  mapStateToProps,
  {}
)(GUIEditor);
