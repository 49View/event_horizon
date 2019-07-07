import axios from "axios";
import { setAlert } from "./alert";
import {
  GET_ENTITIES,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  ENTITIES_PARTIAL_SEARCH_ERROR,
  ENTITY_ERROR,
  DELETE_ENTITY,
  ADD_ENTITY,
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
    const res = await axios.get(`/entities/content/byId/${entitySource.id}`, {
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
    dispatch(setAlert("Thank you for visiting Park Hill School", "success"));
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response, status: err.response }
    });
  }
};

// Delete post
export const deleteEntry = id => async dispatch => {
  try {
    await axios.delete(`/api/entries/${id}`);

    dispatch({
      type: DELETE_ENTITY,
      payload: id
    });

    dispatch(setAlert("Visitor Removed", "success"));
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

  console.log("New Entity: ", entity);

  try {
    const res = await axios.post("/entities", entity, config);

    dispatch({
      type: ADD_ENTITY,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};
