import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import FontEditor from "./entities/FontEditor";

const DashboardFonts = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <FontEditor />
    </div>
  );
};

export default connect()(DashboardFonts);
