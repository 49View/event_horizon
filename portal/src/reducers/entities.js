import {
  GET_ENTITIES,
  GET_APPS,
  GET_MATERIALS_META,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  UPDATE_REPLACE_MATERIAL_PARTIAL_SEARCH,
  ENTITY_ERROR,
  DELETE_ENTITY,
  ADD_ENTITY,
  GET_ENTITY,
  GET_ENTITY_LOAD,
  SET_ENTITY_NODES,
  SET_SELECTED_MAT_NAME,
  CLOSE_REPLACE_MATERIAL,
  REPLACE_MATERIAL,
  REPLACE_ENTITY_TAGS,
  CHANGE_MATERIAL_COLOR,
  RESET_CURRENT_ENTITY,
  LOADING_FINISHED
} from "../actions/types";

import { requestAsset } from "../utils/webSocketClient";

const initialState = {
  events: {},
  entries: [],
  entriesFiltered: [],
  matEntries: [],
  matEntriesFiltered: [],
  currentEntity: null,
  replaceMaterialOn: false,
  currentEntityNodes: null,
  group: "material",
  loading: true,
  error: {}
};

export default function(state = initialState, action) {
  const { type, payload, requireWasmUpdate } = action;

  const evaluateTags = sourceTags => {
    let tags = [];
    let c = 0;
    for (const tag of sourceTags) {
      tags.push({ id: c, name: tag });
      c++;
    }
    return tags;
  };

  switch (type) {
    case LOADING_FINISHED:
      return {
        ...state,
        loading: false
      };
    case GET_APPS:
      return {
        ...state,
        entries: payload.data,
        entriesFiltered: payload.data,
        currentEntity: null,
        group: payload.group,
        loading: false
      };
    case GET_ENTITIES:
      return {
        ...state,
        entries: payload.data,
        entriesFiltered: payload.data,
        currentEntity: null,
        group: payload.group,
        loading: false
      };
    case RESET_CURRENT_ENTITY:
      return {
        ...state,
        currentEntity: null
      };
    case GET_MATERIALS_META:
      return {
        ...state,
        matEntries: payload.data,
        matEntriesFiltered: payload.data,
        loading: false
      };
    case REPLACE_MATERIAL:
      return {
        ...state,
        loading: true,
        replaceMaterialOn: false
      };
    case CLOSE_REPLACE_MATERIAL:
      return {
        ...state,
        replaceMaterialOn: false,
        selectedMatName: ""
      };
    case SET_SELECTED_MAT_NAME:
      return {
        ...state,
        replaceMaterialOn: true,
        selectedMatName: payload
      };
    case UPDATE_ENTITIES_PARTIAL_SEARCH:
      let filteredResult = [];
      //   state.entriesFiltered.data.length = 0;
      for (const e of state.entries) {
        if (e.metadata.name.toLowerCase().includes(payload)) {
          filteredResult.push(e);
        }
      }
      return {
        ...state,
        entriesFiltered: filteredResult,
        loading: false
      };
    case UPDATE_REPLACE_MATERIAL_PARTIAL_SEARCH:
      let matFilteredResult = [];
      //   state.entriesFiltered.data.length = 0;
      for (const e of state.matEntries) {
        if (e.metadata.name.toLowerCase().includes(payload)) {
          matFilteredResult.push(e);
        }
      }
      return {
        ...state,
        matEntriesFiltered: matFilteredResult,
        loading: false
      };

    case GET_ENTITY_LOAD:
      return {
        ...state,
        loading: true,
        currentTags: []
      };
    case ADD_ENTITY:
      // sendAssetToDaemonIfNeeded(payload);
      return {
        ...state
      };
    case GET_ENTITY:
      if (requireWasmUpdate) requestAsset(payload);
      return {
        ...state,
        currentEntity: payload,
        currentTags: evaluateTags(payload.entity.metadata.tags),
        replaceMaterialOn: false,
        loading: requireWasmUpdate
      };
    case SET_ENTITY_NODES:
      return {
        ...state,
        loading: false,
        currentEntityNodes: payload
      };
    // case ADD_ENTITY:
    //   return {
    //     ...state,
    //     entries: [...state.entries, payload],
    //     entry: {},
    //     loading: false
    //   };
    case REPLACE_ENTITY_TAGS:
      return {
        ...state,
        currentTags: evaluateTags(payload),
        loading: false
      };
    case DELETE_ENTITY:
      return {
        ...state,
        entries: state.entries.filter(entry => entry._id !== payload),
        entriesFiltered: state.entries.filter(entry => entry._id !== payload),
        currentEntity: null,
        loading: false
      };
    case CHANGE_MATERIAL_COLOR:
      return {
        ...state,
        lastColorChanged: payload
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
