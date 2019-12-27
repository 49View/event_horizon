import React, {Fragment, useCallback} from "react";
import {useDropzone} from "react-dropzone";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";

import {addEntity, addPlaceHolderEntity} from "../../../actions/entities";
import {
    checkFileExtensionsOnEntityGroup, getFileNameExt, getFileNameOnlyNoExt, GroupGeom,
    groupHasCreateEditor,
    groupHasImportFacility
} from "../../../utils/utils";
import {setAlert} from "../../../actions/alert";
import {useSelector, useDispatch} from "react-redux";

const streams = require("memory-streams");
const tar = require("tar-stream");

const chooseFilenameFromMultiFiles = (files, group) => {
    if (group === GroupGeom) {
        for (const file of files) {
            if (getFileNameExt(file.name) === "fbx") {
                return getFileNameOnlyNoExt(file.name) + ".fbx_folder";
            }
        }
    }
    return null;
}

var isArrayBufferSupported = (new Buffer(new Uint8Array([1]).buffer)[0] === 1);

var arrayBufferToBuffer = isArrayBufferSupported ? arrayBufferToBufferAsArgument : arrayBufferToBufferCycle;

function arrayBufferToBufferAsArgument(ab) {
    return new Buffer(ab);
}

function arrayBufferToBufferCycle(ab) {
    var buffer = new Buffer(ab.byteLength);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buffer.length; ++i) {
        buffer[i] = view[i];
    }
    return buffer;
}

const EntityDragAndImport = () => {

        const dispatch = useDispatch();
        const groupSelected = useSelector(state => state.entities.groupSelected);

        const onDrop = useCallback(
            async acceptedFiles => {

                const readFileAsync = (file) => {
                    let reader = new FileReader();
                    return new Promise((resolve, reject) => {
                        reader.onload = () => {
                            console.log("Finished loading ", file.name);
                            resolve(reader.result);
                        };
                        reader.onerror = () => {
                            reader.abort();
                            console.log("Error loading ", file.name);
                            reject("Cannot open file");
                        }
                        reader.readAsArrayBuffer(file);
                    })
                }

                const readFileAsyncS = async (file) => {
                    try {
                        return await readFileAsync(file);
                    } catch (e) {
                        return null;
                    }
                }

                const tarWrite = (tarPack) => {
                    return new Promise((resolve, reject) => {
                        let writer = new streams.WritableStream();
                        tarPack.pipe(writer);
                        tarPack.on("end", resolve(writer.toBuffer()));
                    })
                }

                const singleFileRead = async (acceptedFile) => {
                    // check file dragged has a valid extension for asset type
                    if (checkFileExtensionsOnEntityGroup(groupSelected, acceptedFile.name)) {
                        const fileContent = await readFileAsync(acceptedFile);
                        dispatch(addEntity(acceptedFile.name, fileContent, groupSelected));
                    } else {
                        dispatch(setAlert("Wrong file type for this entity type", "warning"));
                    }
                }

                const multiFileRead = async (acceptedFiles) => {

                    const filename = chooseFilenameFromMultiFiles(acceptedFiles, groupSelected);

                    if (filename == null) {
                        dispatch(setAlert("No files for " + groupSelected + " group has been selected.", "warning"));
                        return;
                    }

                    let abuffers = [];
                    let tarPack = tar.pack();

                    for (let i = 0; i < acceptedFiles.length; i++) {
                        const ab = await readFileAsyncS(acceptedFiles[i]);
                        if (ab) {
                            abuffers.push({
                                name: acceptedFiles[i].name,
                                size: ab.byteLength,
                                data: ab
                            });
                        }
                    }

                    // tarPack.on('error', reject);
                    const packEntry = (err) => {
                        if (err) {
                            console.log("Error");
                            // reject(err);
                        } else if (abuffers.length) {
                            console.log("Buffer length", abuffers.length)
                            const fileEntry = abuffers.pop();
                            let entry = tarPack.entry({name: fileEntry.name, size: fileEntry.size}, packEntry);
                            entry.write(arrayBufferToBuffer(fileEntry.data));
                            entry.end();
                        } else {
                            console.log("Finalise");
                            tarPack.finalize();
                            // const fileContent = tarWrite(tarPack);
                            // dispatch(addEntity(filename, fileContent, groupSelected));
                            // resolve();
                        }
                    }
                    packEntry();
                }

                try {
                    if (acceptedFiles.length === 1) {
                        await singleFileRead(acceptedFiles[0]);
                    } else if (acceptedFiles.length > 1) {
                        await multiFileRead(acceptedFiles);
                    }
                } catch (e) {
                    console.log(e);
                }
            },
            [dispatch, groupSelected]
            )
        ;

        const {getRootProps, getInputProps} = useDropzone({onDrop});
        const addButton = groupHasCreateEditor(groupSelected);
        const importButton = groupHasImportFacility(groupSelected);

        return (
            <Fragment>
                <div className="leftSideBarGroupImport">
                    <ButtonGroup size="sm" type="checkbox">
                        {addButton && (
                            <Button
                                variant="secondary"
                                value={1}
                                onClick={e => {
                                    addPlaceHolderEntity(groupSelected);
                                }}
                            >
                                <i className="fas fa-plus"></i>
                            </Button>
                        )}
                        {importButton && (
                            <Button variant="secondary" value={2}>
                                <div {...getRootProps({className: "dropzoneNoHMargins"})}>
                                    <input {...getInputProps()} />
                                    <span>
                  <i className="fas fa-upload"/>
                </span>
                                </div>
                            </Button>
                        )}
                    </ButtonGroup>
                </div>
            </Fragment>
        );
    }
;


export default EntityDragAndImport;
