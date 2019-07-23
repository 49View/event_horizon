import React, { Fragment } from "react";
import { connect } from "react-redux";
import EntityTypeTaskbar from "./entities/EntityTypeTaskbar";
import Entries from "./entities/Entries";
import GeomEditor from "./entities/GeomEditor";

const DashboardGeoms = () => {
  console.log("DashboardGeoms render");
  return (
    <Fragment>
      <div className="dashboardContainer">
        <EntityTypeTaskbar />
        <Entries />
        <GeomEditor />
      </div>
    </Fragment>
  );
};

export default connect()(DashboardGeoms);
