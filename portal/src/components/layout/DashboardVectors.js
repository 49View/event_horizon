import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import VectorEditor from "./entities/ColorEditor";

const DashboardVectors = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <VectorEditor />
    </div>
  );
};

export default connect()(DashboardVectors);
