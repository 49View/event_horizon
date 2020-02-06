import axios from "axios";
import {setAlert} from "./alert";
import {
  CHANGE_MATERIAL_COLOR,
  DELETE_ENTITY,
  ENTITIES_PARTIAL_SEARCH_ERROR,
  ENTITY_ERROR,
  GET_ENTITIES,
  GET_ENTITY,
  GET_ENTITY_LIST_PRELOAD,
  GET_ENTITY_LOAD,
  GET_METADATA_LIST,
  LOADING_FINISHED,
  REPLACE_ENTITY_TAGS,
  REPLACE_MATERIAL,
  SET_ENTITY_NODES,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  UPDATE_METADATA_LIST_PARTIAL_SEARCH
} from "./types";
import store from "../store";
import {wscSend} from "../utils/webSocketClient";

// Get entries
export const getEntitiesOfGroup = (group, project) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LIST_PRELOAD,
      payload: group
    });

    let res = null;
    let dtype = GET_ENTITIES;
    res = await axios.get(`/api/entities/metadata/list/${group}/${project}`);
    dispatch({
      type: dtype,
      payload: {data: res.data, group: group}
    });
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};

export const changeEntitiesGroup = (group, project) => async dispatch => {
  try {
    // dispatch({
    //   type: RESET_CURRENT_ENTITY,
    //   payload: null
    // });
    dispatch(getEntitiesOfGroup(group, project));
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
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
    const matId = entity.name;
    const entityId = state.entities.currentEntity.entity.name;
    const sourceId = state.entities.selectedModalEntityName;
    wscSend("ReplaceMaterialOnCurrentObject", {
      mat_id: matId, //entity._id,
      entity_id: entityId,
      source_id: sourceId
    });

    const body = {
      sourceEntity: entityId,
      sourceRemap: sourceId,
      destRemap: matId
    };

    await axios.put(`/api/entities/remaps`, body, {
      headers: {
        "Content-Type": "application/json"
      }
    });

  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};

export const sendMaterialPropertyChange = (matName, matId, value) => {
  try {
    const [propertyStr, valueType] = matName.split("-", 2);
    wscSend("ChangeMaterialProperty", {
      mat_id: matId,
      property_id: propertyStr,
      value_str: value,
      value_type: valueType
    });
  } catch (error) {
    console.log(error);
  }
};

export const updateTextureParameterOnMaterial = (entity, props) => async dispatch => {
  try {
    console.log("Medatadat: ", entity.metadata, "With Props ", props);
    sendMaterialPropertyChange(props.sourceEntityName + "-string", props.fatherEntityName, entity._id);
    // dispatch({
    //   type: REPLACE_MATERIAL,
    //   payload: null
    // });
    // const state = store.getState();
    // const matId = entity.name;
    // const entityId = state.entities.currentEntity.entity.name;
    // const sourceId = state.entities.selectedModalEntityName;
    // wscSend("ReplaceMaterialOnCurrentObject", {
    //   mat_id: matId, //entity._id,
    //   entity_id: entityId,
    //   source_id: sourceId
    // });
    //
    // const body = {
    //   sourceEntity: entityId,
    //   sourceRemap: sourceId,
    //   destRemap: matId
    // };
    //
    // await axios.put(`/api/entities/remaps`, body, {
    //   headers: {
    //     "Content-Type": "application/json"
    //   }
    // });

  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};

export const getMetadataListOf = (group, project) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: group
    });

    const res = await axios.get(`/api/entities/metadata/list/${group}`);

    dispatch({
      type: GET_METADATA_LIST,
      payload: {data: res.data}
    });
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
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
    console.log(err);
    store.dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
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
      payload: {msg: err.response}
    });
  }
};

export const updateMetadataListPartialSearch = partialString => dispatch => {
  try {
    dispatch({
      type: UPDATE_METADATA_LIST_PARTIAL_SEARCH,
      payload: partialString
    });
  } catch (err) {
    dispatch({
      type: ENTITIES_PARTIAL_SEARCH_ERROR,
      payload: {msg: err.response}
    });
  }
};

export const changeMaterialPropery = (matName, matId, value) => dispatch => {
  try {
    console.log("Change material property of " + matName + " that has ID of " + matId);
    sendMaterialPropertyChange(matName, matId, value);
    dispatch({type: CHANGE_MATERIAL_COLOR, payload: value});
  } catch (error) {
    console.log(error);
  }
};

export const wasmClientFinishedLoadingData = data => dispatch => {
  dispatch({
    type: LOADING_FINISHED,
    payload: data
  });
};

// Get entity
export const getFullEntity = entitySource => async dispatch => {
  try {

    window.Module.addScriptLine(
      `rr.addSceneObject( "${entitySource._id}", "${entitySource.group}", "${entitySource.hash}" )`
    );

    dispatch({
      type: GET_ENTITY_LOAD,
      payload: entitySource.name
    });
    //
    // const requireWasmUpdate = entitySource.group !== GroupScript;
    // // Get dependencies for
    // let deps = {};
    // let fullData = null;
    // let responseTypeValue = "arraybuffer";
    // if (entitySource.group === GroupMaterial) {
    //   responseTypeValue = "json";
    // }
    //
    // // eslint-disable-next-line
    // for (const depElem of entitySource.deps) {
    //   // eslint-disable-next-line
    //   for (const depValue of depElem.value) {
    //     const res = await axios.get(`/api/entities/content/byfsid/${depValue}`, {
    //       responseType: "arraybuffer"
    //     });
    //     deps[depValue] = URL.createObjectURL(new Blob([res.data]));
    //   }
    // }
    //
    // fullData = await axios.get(`/api/entities/metadata/byId/${entitySource._id}`, {
    //   responseType: responseTypeValue
    // });
    //
    // const entityFull = {
    //   entity: entitySource,
    //   deps: deps,
    //   blobURL:
    //     responseTypeValue === "arraybuffer"
    //       ? URL.createObjectURL(new Blob([fullData.data]))
    //       : null,
    //   jsonRet: responseTypeValue === "arraybuffer" ? null : fullData.data
    // };
    //
    dispatch({
      type: GET_ENTITY,
      payload: {entity: entitySource},
    });

  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
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

    await axios.put(`/api/entities/metadata/addtags/${id}`, bodyTags, config);

    dispatch({
      type: REPLACE_ENTITY_TAGS,
      payload: tags
    });
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response, status: err.response}
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
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};

const placeHolderEntityMaker = group => {
  return "entities/placeholder/" + group;
};

// Add post
export const addEntity = (fileName, fileData, group) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: fileName
    });

    const octet = {
      headers: {
        "Content-Type": "application/octet-stream"
      }
    };
    const urlEnc = encodeURIComponent(fileName);
    console.log("Url encoded resource: ", urlEnc);
    await axios.post("/api/fs/entity_to_elaborate/" + group + "/" + urlEnc,
      fileData,
      octet
    );
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};

export const addPlaceHolderEntity = group => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: group
    });

    const res = await axios.post(placeHolderEntityMaker(group));

    const entityFull = {
      entity: res.data,
      blobURL: null
    };

    dispatch({
      type: GET_ENTITY,
      payload: entityFull,
      requirePlaceHolder: true
    });
  } catch (err) {
    console.log(err);
    dispatch({
      type: ENTITY_ERROR,
      payload: {msg: err.response}
    });
  }
};
