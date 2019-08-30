import React, { useState } from "react";
import { connect } from "react-redux";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";
import { alphaBool } from "../../../utils/utils";

const SourceControlToolbar = () => {
  const [useSkybox, setUseSkybox] = useState(true);
  const [useVignette, setVignette] = useState(true);
  // const [useFilmGrain, setFilmGrain] = useState(true);
  // const [useBloom, setBloom] = useState(true);
  // const [useDOF, setDOF] = useState(true);
  // const [useSSAO, setSSAO] = useState(true);

  return <div className="source_controls-a"></div>;
};

export default connect()(SourceControlToolbar);
