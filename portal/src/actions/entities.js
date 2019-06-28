import axios from "axios";
import { setAlert } from "./alert";
import {
  GET_ENTITIES,
  GET_ENTITIES_DAILY,
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

    dispatch({
      type: GET_ENTITIES,
      payload: res
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get entries
export const getEntriesDaily = () => async dispatch => {
  try {
    const res = await axios.get("/api/entries/dailycutoff");

    dispatch({
      type: GET_ENTITIES_DAILY,
      payload: res.data
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
export const addEntry = formData => async dispatch => {
  const config = {
    headers: {
      "Content-Type": "application/json"
    }
  };

  try {
    const res = await axios.post("/api/entries", formData, config);

    dispatch({
      type: ADD_ENTITY,
      payload: res.data
    });

    dispatch(setAlert("Welcome to Park Hill " + res.data.name, "success"));
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get post
export const getEntry = id => async dispatch => {
  try {
    const res = await axios.get(`/api/entries/${id}`);

    dispatch({
      type: GET_ENTITY,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: ENTITY_ERROR,
      payload: { msg: err.response }
    });
  }
};
