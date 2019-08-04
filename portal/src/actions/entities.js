import axios from "axios";
import { setAlert } from "./alert";
import {
  GET_ENTITIES,
  GET_MATERIALS_META,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  ENTITIES_PARTIAL_SEARCH_ERROR,
  UPDATE_REPLACE_MATERIAL_PARTIAL_SEARCH,
  ENTITY_ERROR,
  DELETE_ENTITY,
  GET_ENTITY,
  GET_ENTITY_LOAD,
  SET_ENTITY_NODES,
  REPLACE_ENTITY_TAGS,
  REPLACE_MATERIAL,
  CHANGE_MATERIAL_COLOR,
  RESET_CURRENT_ENTITY,
  LOADING_FINISHED
} from "./types";
import store from "../store";
import { wscSend } from "../utils/webSocketClient";

// Get entries
export const getEntitiesOfGroup = (group, project) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const res = await axios.get(`/entities/metadata/list/${group}/${project}`);

    dispatch({ type: GET_ENTITIES, payload: { data: res.data, group: group } });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const changeEntitiesGroup = (group, project) => async dispatch => {
  try {
    dispatch({
      type: RESET_CURRENT_ENTITY,
      payload: null
    });
    dispatch(getEntitiesOfGroup(group, project));
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const replaceMaterial = entity => async dispatch => {
  try {
    dispatch({
      type: REPLACE_MATERIAL,
      payload: null
    });
    // dispatch({
    //   type: REPLACE_MATERIAL,
    //   payload: null
    // });
    const state = store.getState();
    wscSend("ReplaceMaterialOnCurrentObject", {
      mat_id: entity.metadata.name,
      entity_id: state.entities.currentEntity.entity.metadata.name,
      source_id: state.entities.selectedMatName
    });

    // dispatch({
    //   type: GET_MATERIALS_META,
    //   payload: { data: res.data }
    // });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const getAllMaterialsMeta = project => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const res = await axios.get(`/entities/metadata/list/material/${project}`);

    dispatch({
      type: GET_MATERIALS_META,
      payload: { data: res.data }
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const setEntityNodes = nodes => {
  try {
    store.dispatch({
      type: SET_ENTITY_NODES,
      payload: nodes
    });
  } catch (err) {
    store.dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get entries partial search
export const updateEntriesPartialSearch = partialString => dispatch => {
  try {
    dispatch({
      type: UPDATE_ENTITIES_PARTIAL_SEARCH,
      payload: partialString
    });
  } catch (err) {
    dispatch({
      type: ENTITIES_PARTIAL_SEARCH_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const updateReplaceMaterialPartialSearch = partialString => dispatch => {
  try {
    dispatch({
      type: UPDATE_REPLACE_MATERIAL_PARTIAL_SEARCH,
      payload: partialString
    });
  } catch (err) {
    dispatch({
      type: ENTITIES_PARTIAL_SEARCH_ERROR,
      payload: { msg: err.response }
    });
  }
};

export const changeMaterialPropery = event => dispatch => {
  try {
    const [propertyStr, valueType] = event.target.name.split("-", 2);
    wscSend("ChangeMaterialProperty", {
      mat_id: event.target.id,
      property_id: propertyStr,
      value_str: event.target.value,
      value_type: valueType
    });
    dispatch({ type: CHANGE_MATERIAL_COLOR, payload: event.target.value });
  } catch (error) {
    console.log(error);
  }
};

export const wasmClientFinishedLoadingData = () => dispatch => {
  dispatch({
    type: LOADING_FINISHED,
    payload: null
  });
};

// Get entity
export const getFullEntity = entitySource => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const requireWasmUpdate =
      entitySource.group === "geom" || entitySource.group === "material";
    // Get dependencies for
    let deps = {};

    for (const depElem of entitySource.metadata.deps) {
      for (const depValue of depElem.value) {
        const res = await axios.get(`/entities/content/byHash/${depValue}`, {
          responseType: "arraybuffer"
        });
        deps[depValue] = URL.createObjectURL(new Blob([res.data]));
      }
    }

    let responseTypeValue = "arraybuffer";
    if (entitySource.group === "material") {
      responseTypeValue = "json";
    }

    const res = await axios.get(`/entities/content/byId/${entitySource._id}`, {
      responseType: responseTypeValue
    });

    const entityFull = {
      entity: entitySource,
      deps: deps,
      blobURL:
        responseTypeValue === "arraybuffer"
          ? URL.createObjectURL(new Blob([res.data]))
          : null,
      jsonRet: responseTypeValue === "arraybuffer" ? null : res.data
    };

    if (entitySource.group === "app") {
      dispatch({
        type: LOADING_FINISHED,
        payload: null
      });
    } else {
      dispatch({
        type: GET_ENTITY,
        payload: entityFull,
        requireWasmUpdate: requireWasmUpdate
      });
    }
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// checkout visitor
export const addTagsToEntity = (id, tags) => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };

    const bodyTags = {
      tags: tags
    };

    await axios.put(`/entities/metadata/addtags/${id}`, bodyTags, config);

    dispatch({
      type: REPLACE_ENTITY_TAGS,
      payload: tags
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response, status: err.response }
    });
  }
};

export const deleteEntity = id => async dispatch => {
  try {
    await axios.delete(`/entities/${id}`);

    dispatch({
      type: DELETE_ENTITY,
      payload: id
    });

    dispatch(
      setAlert("Asset deleted, hope you are not going to regret it!", "success")
    );
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Add post
export const addEntity = entity => async dispatch => {
  try {
    const config = {
      headers: {
        "Content-Type": "application/json"
      }
    };
    const res = await axios.post("/entities", entity, config);
    const fullres = await axios.get(`/entities/content/byId/${res.data._id}`, {
      responseType: "arraybuffer"
    });
    const entityFull = {
      entity: res.data,
      blobURL: URL.createObjectURL(new Blob([fullres.data]))
    };

    dispatch({
      type: GET_ENTITY,
      payload: entityFull
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};
