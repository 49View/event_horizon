import React, { Fragment } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import { getEntitiesOfGroup } from "../../actions/entities";
import { decode } from "base64-arraybuffer";

const Dashboard = ({ userstate, entities, getEntitiesOfGroup }) => {
  // console.log(userstate);

  // useEffect(() => {
  //   if (userstate.userdata && userstate.userdata.project !== "") {
  //     getEntitiesOfGroup("image", userstate.userdata.project);
  //   }
  // }, [getEntitiesOfGroup, userstate]);

  if (!userstate.isAuthenticated) {
    return <Redirect to="/" />;
  }

  if (
    userstate.userdata &&
    (userstate.userdata.project === null || userstate.userdata.project === "")
  ) {
    return (
      <Fragment>
        <div>You don't seem to have any project assigned yet.</div>
      </Fragment>
    );
  }

  let entitiesRes = [];
  if (entities && entities.length > 0) {
    entities.map(e => {
      const b64 = decode(e.metadata.thumb);
      const bb = new Blob([b64]);
      entitiesRes.push({
        name: e.metadata.name,
        thumb: URL.createObjectURL(bb)
      });
      return 0;
    });
  }

  const viewMore = group => () => {
    let groupId = "material";
    if (group === "Images") groupId = "image";
    getEntitiesOfGroup(groupId, userstate.userdata.project);
    // console.log(group);
  };

  const leftSideEntry = (icon, text) => (
    <div className="leftSideBarGroup">
      <a onClick={viewMore(text)} href="#!">
        <div className="leftSideBarIcon">
          <i className={icon} />
        </div>
        <div className="leftSideBarText"> {text}</div>
      </a>
    </div>
  );

  const searchBox = (
    <Fragment>
      <div className="entitiesSearchBox">
        <input type="text" id="search-bar" placeholder="Search for..." />
        <a href="#!" className="search-icon">
          <i className="fas fa-search" />
        </a>
      </div>
    </Fragment>
  );

  return (
    <Fragment>
      <div className="entitiesContainer">
        {searchBox}
        <div className="leftSideBar">
          {leftSideEntry("fas fa-cube", "Objects")}
          {leftSideEntry("fas fa-code-branch", "Materials")}
          {leftSideEntry("fas fa-images", "Images")}
          {leftSideEntry("fas fa-font", "Fonts")}
          {leftSideEntry("fas fa-vector-square", "Profiles")}
          {leftSideEntry("fas fa-brush", "Colors")}
        </div>
        <div className="mainEntityArea">
          {entitiesRes.map(entry => (
            <div className="EntityThumbnail">
              <div className="EntityThumbnailInset">
                <img width="64" height="64" src={entry.thumb} alt="" />
              </div>
              <div className="EntityThumbnailText">{entry.name}</div>
            </div>
          ))}
        </div>
      </div>
    </Fragment>
  );
};

Dashboard.propTypes = {
  // setAlert: PropTypes.func.isRequired,
  userstate: PropTypes.object,
  entities: PropTypes.array,
  getEntitiesOfGroup: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
  userstate: state.auth,
  entities: state.entities.entries.data
});

export default connect(
  mapStateToProps,
  { getEntitiesOfGroup }
)(Dashboard);
