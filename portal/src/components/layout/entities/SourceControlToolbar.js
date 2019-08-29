import React, { useState } from "react";
import { connect } from "react-redux";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";
import { alphaBool } from "../../../utils/utils";

const SourceControlToolbar = () => {
  const [useSkybox, setUseSkybox] = useState(true);
  const [useVignette, setVignette] = useState(true);
  const [useFilmGrain, setFilmGrain] = useState(true);
  const [useBloom, setBloom] = useState(true);
  const [useDOF, setDOF] = useState(true);
  const [useSSAO, setSSAO] = useState(true);

  return (
    <div className="source_controls-a">
      <ButtonGroup size="sm">
        <Button
          variant="secondary"
          value={1}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useSkybox(" + alphaBool(useSkybox) + ")"
            );
            setUseSkybox(!useSkybox);
          }}
        >
          <i className="fas fa-play"></i>
        </Button>
        <Button
          variant="secondary"
          value={2}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useVignette(" + alphaBool(useVignette) + ")"
            );
            setVignette(!useVignette);
          }}
        >
          <i className="fas fa-save"></i>
        </Button>
      </ButtonGroup>
    </div>
  );
};

export default connect()(SourceControlToolbar);
