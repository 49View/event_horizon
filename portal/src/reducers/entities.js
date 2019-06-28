import {
  GET_ENTITIES,
  GET_ENTITIES_DAILY,
  ENTITY_ERROR,
  DELETE_ENTITY,
  ADD_ENTITY,
  GET_ENTITY,
  CHECKOUT_ENTITY
} from "../actions/types";

const initialState = {
  entries: [],
  loading: true,
  error: {}
};

export default function(state = initialState, action) {
  const { type, payload } = action;

  switch (type) {
    case GET_ENTITIES:
      return {
        ...state,
        entries: payload,
        loading: false
      };
    case GET_ENTITIES_DAILY:
      return {
        ...state,
        entries_daily: payload,
        loading: false
      };
    case GET_ENTITY:
      return {
        ...state,
        entry: payload,
        loading: false
      };
    case ADD_ENTITY:
      return {
        ...state,
        entries: [...state.entries, payload],
        entry: {},
        loading: false
      };
    case CHECKOUT_ENTITY:
      return {
        ...state,
        entries: state.entries.map(entry =>
          entry._id === payload._id
            ? { ...entry, timeout: payload.timeout }
            : entry
        ),
        // entries: [...state.entries],
        loading: false
      };
    case DELETE_ENTITY:
      return {
        ...state,
        entries: state.entries.filter(entry => entry._id !== payload),
        loading: false
      };
    case ENTITY_ERROR:
      return {
        ...state,
        error: payload,
        loading: false
      };
    default:
      return state;
  }
}
