import React from "react";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import ImageEditor from "./entities/ImageEditor";

const DashboardImages = () => {
  return (
    <div className="dashboardContainer">
      <Entries cname="thumbs-a thumbsEntityArea" />
      <ImageEditor />
    </div>
  );
};

export default connect()(DashboardImages);
