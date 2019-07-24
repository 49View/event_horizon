import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import GeomEditor from "./entities/GeomEditor";

const DashboardGeoms = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <GeomEditor />
    </div>
  );
};

export default connect()(DashboardGeoms);
