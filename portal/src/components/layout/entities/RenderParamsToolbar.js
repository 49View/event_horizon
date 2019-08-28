import React, { useState } from "react";
import { connect } from "react-redux";
import ToggleButtonGroup from "react-bootstrap/ToggleButtonGroup";
import ToggleButton from "react-bootstrap/ToggleButton";
import { alphaBool } from "../../../utils/utils";

const RenderParamsToolbar = () => {
  const [useSkybox, setUseSkybox] = useState(true);
  const [useVignette, setVignette] = useState(true);
  const [useFilmGrain, setFilmGrain] = useState(true);
  const [useBloom, setBloom] = useState(true);
  const [useDOF, setDOF] = useState(true);
  const [useSSAO, setSSAO] = useState(true);

  return (
    <div className="renderParams-a">
      <ToggleButtonGroup size="sm" type="checkbox">
        <ToggleButton
          variant="secondary"
          value={1}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useSkybox(" + alphaBool(useSkybox) + ")"
            );
            setUseSkybox(!useSkybox);
          }}
        >
          <i className="fas fa-globe"></i>
        </ToggleButton>
        <ToggleButton
          variant="secondary"
          value={2}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useVignette(" + alphaBool(useVignette) + ")"
            );
            setVignette(!useVignette);
          }}
        >
          <i className="fas fa-dot-circle"></i>
        </ToggleButton>
        <ToggleButton
          variant="secondary"
          value={3}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useBloom(" + alphaBool(useBloom) + ")"
            );
            setBloom(!useBloom);
          }}
        >
          <i className="fas fa-sun"></i>
        </ToggleButton>
        <ToggleButton
          variant="secondary"
          value={4}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useFilmGrain(" + alphaBool(useFilmGrain) + ")"
            );
            setFilmGrain(!useFilmGrain);
          }}
        >
          <i className="fas fa-braille"></i>
        </ToggleButton>
        <ToggleButton
          variant="secondary"
          value={5}
          onChange={e => {
            window.Module.addScriptLine("rr.useDOF(" + alphaBool(useDOF) + ")");
            setDOF(!useDOF);
          }}
        >
          <i className="far fa-images"></i>
        </ToggleButton>
        <ToggleButton
          variant="secondary"
          value={6}
          onChange={e => {
            window.Module.addScriptLine(
              "rr.useSSAO(" + alphaBool(useSSAO) + ")"
            );
            setSSAO(!useSSAO);
          }}
        >
          <i className="fas fa-rainbow"></i>
        </ToggleButton>
      </ToggleButtonGroup>
    </div>
  );
};

export default connect()(RenderParamsToolbar);
