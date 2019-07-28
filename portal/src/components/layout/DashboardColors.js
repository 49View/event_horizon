import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import ColorEditor from "./entities/ColorEditor";

const DashboardColors = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <ColorEditor />
    </div>
  );
};

export default connect()(DashboardColors);
