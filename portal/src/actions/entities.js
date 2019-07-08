import axios from "axios";
import { setAlert } from "./alert";
import {
  GET_ENTITIES,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  ENTITIES_PARTIAL_SEARCH_ERROR,
  ENTITY_ERROR,
  DELETE_ENTITY,
  GET_ENTITY,
  CHECKOUT_ENTITY
} from "./types";

// Get entries
export const getEntitiesOfGroup = (group, project) => async dispatch => {
  try {
    const res = await axios.get(`/entities/metadata/list/${group}/${project}`);

    dispatch({ type: GET_ENTITIES, payload: { data: res.data, group: group } });
  } catch (err) {
    dispatch({
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

// Get entity
export const getFullEntity = entitySource => async dispatch => {
  try {
    const res = await axios.get(`/entities/content/byId/${entitySource._id}`, {
      responseType: "arraybuffer"
    });
    const entityFull = {
      entity: entitySource,
      blobURL: URL.createObjectURL(new Blob([res.data]))
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

// checkout visitor
export const checkoutEntry = id => async dispatch => {
  try {
    const res = await axios.put(`/api/entries/checkout/${id}`);

    dispatch({
      type: CHECKOUT_ENTITY,
      payload: res.data
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
  const config = {
    headers: {
      "Content-Type": "application/json"
    }
  };

  try {
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
