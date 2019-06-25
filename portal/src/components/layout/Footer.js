import React from "react";

const Footer = () => {
  return (
    <nav className="footer">
      <div className="container-footer">
        <div className="smallprint">
          Park Hill Preparatory School is committed to safeguarding and
          promoting the welfare of children. All staff are trained in this area.
          If at any time during your visit to this school you become aware of
          any actual or potential risks to the safety or welfare of children,
          you have a duty to report your concern to the member of staff
          supervising you. In addition, when you return your visitors badge
          please tell the reception staff you have a concern. All concerns will
          be followed thorugh in accordance with our published safeguarding
          policy. This is published on our website and is also available as a
          hard copy on request.
        </div>
        <div className="smallprintimage">
          <img src="person1.jpg" alt="" />
        </div>
        <div className="smallprintprofile">
          <span className="smallprintprofilebold">Mr Alistair Bond</span>
          <br />
          <span>Headmaster</span>
          <br />
          <span>Designated</span>
          <br />
          <span>Safeguarding Lead</span>
        </div>
        <div className="smallprintimage">
          <img src="person2.png" alt="" />
        </div>
        <div className="smallprintprofile">
          <span className="smallprintprofilebold">Miss Hannah Renfrew</span>
          <br />
          <span>Deputy Designated</span>
          <br />
          <span>Safeguarding Lead</span>
          <br />
        </div>
      </div>
    </nav>
  );
};

export default Footer;
