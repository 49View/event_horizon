import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { Treebeard } from "react-treebeard";
import SmallEntriesDialog from "./SmallEntriesDialog";
import { SET_MODAL_SELECTED_ENTITY_NAME } from "../../../actions/types";
import store from "../../../store";
import { addEntityToAppData } from "../../../actions/entities";

const AppFileHandler = ({ appData, smallEntityModalOn }) => {
  const [data, setData] = useState({});
  const [cursor, setCursor] = useState(false);
  const [currFileHandlerGroup, setCurrFileHandlerGroup] = useState(null);

  const addId = "Add..";

  useEffect(() => {
    if (appData) {
      let lData = {
        name: appData.mKey,
        toggled: true,
        children: []
      };

      const addStyle = {
        color: "yellow"
      };
      const addDecorator = {
        Header: () => {
          return (
            <div style={addStyle}>
              <i className="fas fa-plus-circle"></i> Add
            </div>
          );
        }
      };
      // eslint-disable-next-line
      for (const ent of appData.entities) {
        let entChildren = [];
        const keyGroup = ent.key;
        entChildren.push({
          id: keyGroup,
          name: addId,
          decorators: addDecorator
        });
        // eslint-disable-next-line
        for (const ev of ent.value) {
          entChildren.push({
            name: ev
          });
        }
        lData.children.push({
          name: getEntityDBGroup(ent.key),
          children: entChildren
        });
      }
      setData(lData);
    }
  }, [appData]);

  const getEntityDBGroup = name => {
    return name + (name.endsWith("s") ? "es" : "s");
  };

  const onToggle = (node, toggled) => {
    if (node.name === addId) {
      setCurrFileHandlerGroup(node.id);
      store.dispatch({
        type: SET_MODAL_SELECTED_ENTITY_NAME,
        payload: "Add new entity..."
      });
    }
    if (cursor) {
      cursor.active = false;
    }
    node.active = true;
    if (node.children) {
      node.toggled = toggled;
    }
    setCursor(node);
    setData(Object.assign({}, data));
  };

  return (
    <Fragment>
      {smallEntityModalOn && (
        <SmallEntriesDialog
          group={currFileHandlerGroup}
          onClickCallback={addEntityToAppData}
        />
      )}

      <Treebeard data={data} onToggle={onToggle} />
    </Fragment>
  );
};

AppFileHandler.propTypes = {
  appData: PropTypes.object,
  smallEntityModalOn: PropTypes.bool
};

const mapStateToProps = state => ({
  appData: state.entities.currentEntity.entity,
  smallEntityModalOn: state.entities.smallEntityModalOn
});

export default connect(
  mapStateToProps,
  {}
)(AppFileHandler);
