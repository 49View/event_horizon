import axios from "axios";
import {setAlert} from "./alert";
import {
    CHANGE_MATERIAL_COLOR,
    CLOSE_ENTITIES_MODAL,
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
import {getFileNameOnlyNoExt, GroupGeom, GroupMaterial, GroupScript} from "../utils/utils";

// Get entries
export const getEntitiesOfGroup = (group, project) => async dispatch => {
    try {
        dispatch({
            type: GET_ENTITY_LIST_PRELOAD,
            payload: group
        });

        let res = null;
        let dtype = GET_ENTITIES;
        res = await axios.get(`/entities/metadata/list/${group}/${project}`);
        dispatch({
            type: dtype,
            payload: {data: res.data, group: group}
        });
    } catch (err) {
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
        const matId = entity.metadata.name;
        const entityId = state.entities.currentEntity.entity.metadata.name;
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

        await axios.put(`/entities/remaps`, body, {
            headers: {
                "Content-Type": "application/json"
            }
        });

    } catch (err) {
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
        // const matId = entity.metadata.name;
        // const entityId = state.entities.currentEntity.entity.metadata.name;
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
        // await axios.put(`/entities/remaps`, body, {
        //   headers: {
        //     "Content-Type": "application/json"
        //   }
        // });

    } catch (err) {
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
            payload: null
        });

        const res = await axios.get(`/entities/metadata/list/${group}`);

        dispatch({
            type: GET_METADATA_LIST,
            payload: {data: res.data}
        });
    } catch (err) {
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
        dispatch({type: CLOSE_ENTITIES_MODAL, payload: false});

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
            payload: {msg: err.response}
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

        const requireWasmUpdate = entitySource.group !== GroupScript;
        // Get dependencies for
        let deps = {};
        let fullData = null;
        let responseTypeValue = "arraybuffer";
        if (entitySource.group === GroupMaterial) {
            responseTypeValue = "json";
        }

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

        await axios.put(`/entities/metadata/addtags/${id}`, bodyTags, config);

        dispatch({
            type: REPLACE_ENTITY_TAGS,
            payload: tags
        });
    } catch (err) {
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
        dispatch({
            type: ENTITY_ERROR,
            payload: {msg: err.response}
        });
    }
};

const postEntityMaker = (fileName, project, group, uname, uemail) => {
    return (
        "entities/" +
        fileName +
        "/" +
        encodeURIComponent(project) +
        "/" +
        group +
        "/" +
        encodeURIComponent(uname) +
        "/" +
        encodeURIComponent(uemail)
    );
};

const placeHolderEntityMaker = group => {
    return "entities/placeholder/" + group;
};

// Add post
export const addEntity = (fileName, fileData, group) => async dispatch => {
    try {
        const state = store.getState();
        const project = state.auth.userdata.project;
        const user = state.auth.userdata.user;

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
            payload: {msg: err.response}
        });
    }
};

export const addPlaceHolderEntity = group => async dispatch => {
    try {
        dispatch({
            type: GET_ENTITY_LOAD,
            payload: null
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
        dispatch({
            type: ENTITY_ERROR,
            payload: {msg: err.response}
        });
    }
};
