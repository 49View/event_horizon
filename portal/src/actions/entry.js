import axios from "axios";
import { setAlert } from "./alert";
import {
  GET_ENTRIES,
  GET_ENTRIES_DAILY,
  ENTRY_ERROR,
  DELETE_ENTRY,
  ADD_ENTRY,
  GET_ENTRY,
  CHECKOUT_ENTRY
} from "./types";

// Get entries
export const getEntries = (fromdate, todate) => async dispatch => {
  try {
    const res = await axios.get(`/api/entries/range/${fromdate}/${todate}`);
    //    const res = await axios.get(`/api/entries/range/0/434322323`);

    dispatch({
      type: GET_ENTRIES,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get entries
export const getEntriesDaily = () => async dispatch => {
  try {
    const res = await axios.get("/api/entries/dailycutoff");

    dispatch({
      type: GET_ENTRIES_DAILY,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// checkout visitor
export const checkoutEntry = id => async dispatch => {
  try {
    const res = await axios.put(`/api/entries/checkout/${id}`);

    dispatch({
      type: CHECKOUT_ENTRY,
      payload: res.data
    });
    dispatch(setAlert("Thank you for visiting Park Hill School", "success"));
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
      payload: { msg: err.response, status: err.response }
    });
  }
};

// Delete post
export const deleteEntry = id => async dispatch => {
  try {
    await axios.delete(`/api/entries/${id}`);

    dispatch({
      type: DELETE_ENTRY,
      payload: id
    });

    dispatch(setAlert("Visitor Removed", "success"));
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
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
      type: ADD_ENTRY,
      payload: res.data
    });

    dispatch(setAlert("Welcome to Park Hill " + res.data.name, "success"));
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
      payload: { msg: err.response }
    });
  }
};

// Get post
export const getEntry = id => async dispatch => {
  try {
    const res = await axios.get(`/api/entries/${id}`);

    dispatch({
      type: GET_ENTRY,
      payload: res.data
    });
  } catch (err) {
    dispatch({
      type: ENTRY_ERROR,
      payload: { msg: err.response }
    });
  }
};
