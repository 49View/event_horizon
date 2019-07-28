import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import MaterialEditor from "./entities/MaterialEditor";

const DashboardMaterials = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <MaterialEditor />
    </div>
  );
};

export default connect()(DashboardMaterials);
