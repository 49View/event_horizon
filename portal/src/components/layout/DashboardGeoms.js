import React, { Fragment } from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import GeomEditor from "./entities/GeomEditor";

const DashboardGeoms = () => {
  console.log("DashboardGeoms render");
  return (
    <Fragment>
      <div className="dashboardContainer">
        <Entries />
        <GeomEditor />
      </div>
    </Fragment>
  );
};

export default connect()(DashboardGeoms);
