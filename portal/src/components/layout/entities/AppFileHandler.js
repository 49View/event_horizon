import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { Treebeard } from "react-treebeard";

const AppFileHandler = ({ appData }) => {
  const [data, setData] = useState({});
  const [cursor, setCursor] = useState(false);

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
        entChildren.push({
          name: "add...",
          decorators: addDecorator
        });
        // eslint-disable-next-line
        for (const ev of ent.value) {
          entChildren.push({
            name: ev
          });
        }
        lData.children.push({
          name: ent.key,
          children: entChildren
        });
      }
      setData(lData);
    }
  }, [appData]);

  const onToggle = (node, toggled) => {
    if (cursor.children && cursor.children[0] === node) {
      console.log(node, cursor);
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
      <Treebeard data={data} onToggle={onToggle} />
    </Fragment>
  );
};

AppFileHandler.propTypes = {
  appData: PropTypes.object
};

const mapStateToProps = state => ({
  appData: state.entities.currentEntity.entity
});

export default connect(
  mapStateToProps,
  {}
)(AppFileHandler);
