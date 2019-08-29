import React, { Fragment, useState, useEffect} from "react";
import PropTypes from "prop-types";

import PropTypes from "prop-types";
import { connect } from "react-redux";
import { UnControlled as CodeMirror } from "react-codemirror2";
import SourceControlToolbar from "./SourceControlToolbar";
// import { wscSend } from "../../../utils/webSocketClient";
import Tabs from "react-bootstrap/Tabs";
import Tab from "react-bootstrap/Tab";
import { useIndexedDB } from 'react-indexed-db';

require("codemirror/lib/codemirror.css");
require("codemirror/theme/material.css");
require("codemirror/theme/neat.css");
require("codemirror/mode/lua/lua.js");

const AppEditor = ({
  currentEntity,
  userData
}) => {

  const { getAll, add, update } = useIndexedDB('files');
  const [fileData, setFileData] = useState(null);

  useEffect(() => {
    console.log("prj", userData.project);
    console.log("app", currentEntity.entity.mKey);
    console.log("user", userData.user.email);
    if (userData!==null && userData.project!==null && currentEntity!==null && currentEntity.entity!==null && userData.user!==null && userData.user.email!==null) {
      getAll().then(filesFromDb => {
        console.log("FILES:",filesFromDb);
        filesFromDb.some(fileFromDb => {
          console.log("prj", userData.project, fileFromDb.project);
          console.log("app", currentEntity.entity.mKey, fileFromDb.app);
          console.log("file", "main.lua", fileFromDb.file);
          console.log("user", userData.user.email, fileFromDb.user);
          if (fileFromDb.project===userData.project && fileFromDb.app===currentEntity.entity.mKey && fileFromDb.file==="main.lua" && fileFromDb.user===userData.user.email) {
            console.log("FILE FOUND");
            setFileData(fileFromDb);            
            return true;            
          }
        });
      });
    }
  }, []);
 
  const handleClick = (fileContent) => {
    console.log("CURRENT FILE:", fileData);
    if (fileData===null) {
      console.log("ADD NEW FILE");
      add({ project: userData.project, app: currentEntity.entity.mKey, file: 'main.lua', user: userData.user.email, content: fileContent}).then(
        event => {
          console.log('FILE ADDED: ', event);
          let updatedFileData ={ id: event, project: userData.project, app: currentEntity.entity.mKey, file: 'main.lua', user: userData.user.email, content: fileContent}
          console.log("Updated file data:", updatedFileData);
          setFileData(updatedFileData);
        },
        error => {
          console.log(error);
        }
      );
    } else {
      console.log("UPDATE FILE");
        update({ id: fileData.id, project: userData.project, app: currentEntity.entity.mKey, file: 'main.lua', user: userData.user.email, content: fileContent}).then(
        event => {
          console.log('FILE UPDATED: ', event);
        },
        error => {
          console.log(error);
        }
      );
    }
  };

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
    <div className="appdataquad">
      <CodeMirror
        value={ fileData===null?null:fileData.content}
        className="react-codemirror2 appdataquad"
        options={{
          mode: "lua",
          theme: "material",
          lineNumbers: true
        }}
        onKeyPress={(editor, event) => {
          if (event.code === "Enter" && event.ctrlKey === true) {
            const content = editor.getValue();
            handleClick(content);
            window.Module.addScriptLine(content);
          }
        }}
      />
    </div>
  );
};

AppEditor.propTypes = {
  currentEntity: PropTypes.object,
  userData: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  userData: state.auth.userdata
});

export default connect(
  mapStateToProps,
  {}
)(AppEditor);
