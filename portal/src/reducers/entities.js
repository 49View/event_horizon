import {
  GET_ENTITIES,
  CLEAR_ENTITIES,
  GET_APPS,
  GET_METADATA_LIST,
  UPDATE_ENTITIES_PARTIAL_SEARCH,
  UPDATE_METADATA_LIST_PARTIAL_SEARCH,
  ENTITY_ERROR,
  DELETE_ENTITY,
  ADD_ENTITY,
  GET_ENTITY,
  SET_ENTITY_APP_NAME,
  GET_ENTITY_LOAD,
  GET_ENTITY_LIST_PRELOAD,
  SET_ENTITY_NODES,
  SET_MODAL_SELECTED_ENTITY_NAME,
  CLOSE_ENTITIES_MODAL,
  REPLACE_MATERIAL,
  REPLACE_ENTITY_TAGS,
  CHANGE_MATERIAL_COLOR,
  RESET_CURRENT_ENTITY,
  LOADING_FINISHED
} from "../actions/types";

import { placeHolderAsset } from "../utils/webSocketClient";

const initialState = {
  events: {},
  entries: [],
  entriesFiltered: [],
  metadataList: {
    entries: [],
    filtered: [],
    fatherEntityName: "",
    sourceEntityName: "",
    enable: false,
    group: "",
    onClickCallback: null
  },
  currentEntity: null,
  currentEntityData: null,
  currentEntityNodes: [],
  appKey: "",
  group: "",
  groupSelected: "",
  loading: false,
  loadingFilename: "",
  error: {}
};

export default function(state = initialState, action) {
  const { type, payload, requireWasmUpdate, requirePlaceHolder } = action;

  const evaluateTags = sourceTags => {
    let tags = [];
    let c = 0;
    // eslint-disable-next-line
    for (const tag of sourceTags) {
      tags.push({ id: c, name: tag });
      c++;
    }
    return tags;
  };

  switch (type) {
    case CLEAR_ENTITIES:
      return {
        ...state,
        entries: [],
        entriesFiltered: [],
        currentEntity: null,
        currentEntityData: null,
        group: ""
      };
    case LOADING_FINISHED:
      return {
        ...state,
        currentEntityData: payload,
        loading: false
      };
    case GET_APPS:
      return {
        ...state,
        entries: payload.data,
        entriesFiltered: payload.data,
        currentEntity: null,
        currentEntityData: null,
        group: payload.group,
        loading: false
      };
    case GET_ENTITIES:
      return {
        ...state,
        entries: payload.data,
        entriesFiltered: payload.data,
        // group: payload.group,
        loading: false
      };
    case RESET_CURRENT_ENTITY:
      return {
        ...state,
        entries: [],
        entriesFiltered: [],
        group: "",
        currentEntity: null,
        currentEntityData: null
      };
    case GET_METADATA_LIST:
      return {
        ...state,
        metadataList: {...state.metadataList, entries: payload.data, filtered: payload.data },
        loading: false
      };
    case REPLACE_MATERIAL:
      return {
        ...state,
        loading: true,
        metadataList: {...state.metadataList, enable: false }
      };
    case CLOSE_ENTITIES_MODAL:
      return {
        ...state,
        metadataList: {...state.metadataList, enable: false, sourceEntityName: "" }
      };
    case SET_MODAL_SELECTED_ENTITY_NAME:
      return {
        ...state,
        metadataList: {...state.metadataList,
          enable: true,
          fatherEntityName: payload.fatherEntityName,
          sourceEntityName: payload.selectedModalEntityName,
          group: payload.group,
          onClickCallback: payload.onClickCallback,
         }
      };
    case UPDATE_ENTITIES_PARTIAL_SEARCH:
      let filteredResult = [];
      //   state.entriesFiltered.data.length = 0;
      // eslint-disable-next-line
      for (const e of state.entries) {
        if (e.name.toLowerCase().includes(payload)) {
          filteredResult.push(e);
        }
      }
      return {
        ...state,
        entriesFiltered: filteredResult,
        loading: false
      };
    case UPDATE_METADATA_LIST_PARTIAL_SEARCH:
      let matFilteredResult = [];
      // eslint-disable-next-line
      for (const e of state.metadataList.entries) {
        if (e.name.toLowerCase().includes(payload)) {
          matFilteredResult.push(e);
        }
      }
      return {
        ...state,
        metadataList: {...state.metadataList, filtered: matFilteredResult },
        loading: false
      };

    case GET_ENTITY_LIST_PRELOAD:
      return {
        ...state,
        groupSelected: payload,
        loading: true,
        currentTags: [],
        entriesFiltered: [],
        entries: []
      };
    case GET_ENTITY_LOAD:
      return {
        ...state,
        loading: true,
        loadingFileName: payload
        // currentTags: []
      };
    case GET_ENTITY:
      if (requirePlaceHolder) placeHolderAsset(payload.entity.group);
      return {
        ...state,
        currentEntity: payload,
        group: payload.entity.group,
        currentTags: evaluateTags(payload.entity.tags),
        metadataList: {...state.metadataList, enable: false },
        loading: false
      };
    case SET_ENTITY_APP_NAME:
      return {
        ...state,
        appKey: payload
      };
    case SET_ENTITY_NODES:
      return {
        ...state,
        loading: false,
        currentEntityNodes: payload.mrefs,
        currentEntityData: null
      };
    case ADD_ENTITY:
      return {
        ...state,
        entries: [...state.entries, payload],
        entriesFiltered: [...state.entriesFiltered, payload],
        loading: false
      };
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
        currentEntityData: null,
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
