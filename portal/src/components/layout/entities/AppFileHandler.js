import React, { Fragment, useState, useEffect } from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { Treebeard } from "react-treebeard";

// const dataExample = {
//   name: "root",
//   toggled: true,
//   children: [
//     {
//       name: "parent",
//       children: [{ name: "child1" }, { name: "child2" }]
//     },
//     {
//       name: "loading parent",
//       loading: true,
//       children: []
//     },
//     {
//       name: "parent",
//       children: [
//         {
//           name: "nested parent",
//           children: [{ name: "nested child 1" }, { name: "nested child 2" }]
//         }
//       ]
//     }
//   ]
// };

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

      // eslint-disable-next-line
      for (const ent of appData.entities) {
        let entChildren = [];
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
