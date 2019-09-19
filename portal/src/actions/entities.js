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
  SET_ENTITY_APP_NAME,
  GET_ENTITY_LOAD,
  GET_ENTITY_LIST_PRELOAD,
  SET_ENTITY_NODES,
  REPLACE_ENTITY_TAGS,
  REPLACE_MATERIAL,
  CHANGE_MATERIAL_COLOR,
  LOADING_FINISHED,
  CLOSE_ENTITIES_MODAL
} from "./types";
import store from "../store";
import { wscSend } from "../utils/webSocketClient";
import {
  getFileNameOnlyNoExt,
  GroupApp,
  GroupGeom,
  GroupImage,
  GroupMaterial
} from "../utils/utils";

// Get entries
export const getEntitiesOfGroup = (group, project) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LIST_PRELOAD,
      payload: null
    });

    let res = null;
    let dtype = GET_ENTITIES;
    res = await axios.get(`/entities/metadata/list/${group}/${project}`);
    dispatch({
      type: dtype,
      payload: { data: res.data, group: group }
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
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
    const matId = entity.metadata.name;
    const entityId = state.entities.currentEntity.entity.metadata.name;
    const sourceId = state.entities.selectedModalEntityName;
    wscSend("ReplaceMaterialOnCurrentObject", {
      mat_id: matId, //entity._id,
      entity_id: entityId,
      source_id: sourceId
    });

    const appKey = state.auth.userdata.project;
    const body = {
      key: appKey,
      matkey: sourceId,
      objkey: entityId,
      value: matId
    };

    await axios.put(`/appdata/matremap`, body, {
      headers: {
        "Content-Type": "application/json"
      }
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

export const getMetadataListOf = (group, project) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const res = await axios.get(`/entities/metadata/list/${group}/${project}`);

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

export const addEntityToAppData = entitySource => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });
    const state = store.getState();
    const appKey = state.entities.currentEntity.entity.mKey;
    const appDataJson = await axios.put(
      `/appdata/${appKey}/${entitySource.group}/${entitySource.metadata.name}`
    );
    dispatch({ type: CLOSE_ENTITIES_MODAL, payload: false });

    const entityFull = {
      entity: appDataJson.data,
      deps: [],
      blobURL: null,
      jsonRet: null
    };

    dispatch({
      type: GET_ENTITY,
      payload: entityFull,
      requireWasmUpdate: false
    });
  } catch (err) {
    console.log("EntityError: ", err.response);
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get entity
export const getFullEntity = entitySource => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const requireWasmUpdate =
      entitySource.group === GroupGeom ||
      entitySource.group === GroupMaterial ||
      entitySource.group === GroupImage;
    // Get dependencies for
    let deps = {};
    let fullData = null;
    let responseTypeValue = "arraybuffer";
    if (
      entitySource.group === GroupMaterial ||
      entitySource.group === GroupApp
    ) {
      responseTypeValue = "json";
    }

    if (entitySource.group !== GroupApp) {
      // eslint-disable-next-line
      for (const depElem of entitySource.metadata.deps) {
        // eslint-disable-next-line
        for (const depValue of depElem.value) {
          const res = await axios.get(`/entities/content/byHash/${depValue}`, {
            responseType: "arraybuffer"
          });
          deps[depValue] = URL.createObjectURL(new Blob([res.data]));
        }
      }

      fullData = await axios.get(`/entities/content/byId/${entitySource._id}`, {
        responseType: responseTypeValue
      });
    } else if (entitySource.group === GroupApp) {
      fullData = entitySource; //await axios.get(`/appdata/${entitySource.mKey}`);
      dispatch({
        type: SET_ENTITY_APP_NAME,
        payload: fullData.mKey
      });
    }

    const entityFull = {
      entity: entitySource,
      deps: deps,
      blobURL:
        responseTypeValue === "arraybuffer"
          ? URL.createObjectURL(new Blob([fullData.data]))
          : null,
      jsonRet: responseTypeValue === "arraybuffer" ? null : fullData.data
    };

    dispatch({
      type: GET_ENTITY,
      payload: entityFull,
      requireWasmUpdate: requireWasmUpdate
    });
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

const postEntityMaker = (fileName, project, group, uname, uemail) => {
  return (
    "entities/" +
    fileName +
    "/" +
    project +
    "/" +
    group +
    "/" +
    uname +
    "/" +
    uemail
  );
};

// Add post
export const addEntity = (
  fileName,
  fileData,
  group,
  project,
  user
) => async dispatch => {
  try {
    dispatch({
      type: GET_ENTITY_LOAD,
      payload: null
    });

    const octet = {
      headers: {
        "Content-Type": "application/octet-stream"
      }
    };
    let res = null;
    if (group === GroupGeom || group === GroupMaterial) {
      const fileext = fileName.split(".").pop();
      if (group === GroupMaterial && fileext === "zip") {
        const fname = getFileNameOnlyNoExt(fileName);
        await axios.post(
          "/entities/multizip/" + fname + "/" + group,
          fileData,
          octet
        );
      } else {
        const urlEnc = encodeURIComponent(
          "elaborate/" + group + "/" + fileName
        );
        console.log("Url encoded resource: ", urlEnc);
        res = await axios.post(
          "fs/entity_to_elaborate/" + group + "/" + urlEnc,
          fileData,
          octet
        );
      }
    } else {
      res = await axios.post(
        postEntityMaker(fileName, project, group, user.name, user.email),
        fileData,
        octet
      );

      const fullres = await axios.get(
        `/entities/content/byId/${res.data._id}`,
        {
          responseType: "arraybuffer"
        }
      );

      const entityFull = {
        entity: res.data,
        blobURL: URL.createObjectURL(new Blob([fullres.data]))
      };

      dispatch({
        type: GET_ENTITY,
        payload: entityFull
      });
    }
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};
