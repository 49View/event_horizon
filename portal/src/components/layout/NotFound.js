import React, { Fragment } from "react";

const NotFound = () => {
  return (
    <Fragment>
      <section className="container">
        <h1 className="x-large text-primary">
          <i className="fas fa-exclamation-triangle" /> Mamma guarda, senza
          mani!!
        </h1>
        <p className="large">Sorry, this page does not exist</p>
      </section>
    </Fragment>
  );
};

export default NotFound;
