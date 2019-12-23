import React, {Fragment, useCallback} from "react";
import PropTypes from "prop-types";
import {connect} from "react-redux";
import {useDropzone} from "react-dropzone";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Button from "react-bootstrap/Button";

import {addEntity, addPlaceHolderEntity} from "../../../actions/entities";
import {
    checkFileExtensionsOnEntityGroup,
    groupHasCreateEditor,
    groupHasImportFacility
} from "../../../utils/utils";
import store from "../../../store";
import {setAlert} from "../../../actions/alert";

const EntityDragAndImport = ({
                                 addEntity,
                                 addPlaceHolderEntity,
                                 groupSelected,
                                 project
                             }) => {

    const onDrop = useCallback(
        acceptedFiles => {

            const readFileAsync = (file) => {
                return new Promise((resolve, reject) => {
                    let reader = new FileReader();
                    reader.onload = () => {
                        resolve(reader.result);
                    };
                    reader.onabort = reject;
                    reader.onerror = reject;
                    reader.readAsArrayBuffer(file);
                })
            }

            const singleFileRead = async (acceptedFile)  => {
                // check file dragged has a valid extension for asset type
                if ( checkFileExtensionsOnEntityGroup(groupSelected, acceptedFile.name) ) {
                    const fileContent = await readFileAsync( acceptedFile );
                    addEntity(acceptedFile.name, fileContent, groupSelected);
                } else {
                    store.dispatch(setAlert("Wrong file type for this entity type", "warning"));
                }
            }

            const multiFileRead = (acceptedFiles) => {
                acceptedFiles.forEach( async (e) => {
                    await readFileAsync(e);
                });
            }

            try {
                if (acceptedFiles.length === 1) {
                    singleFileRead(acceptedFiles[0]);
                } else if (acceptedFiles.length > 1) {
                    multiFileRead(acceptedFiles);
                }
            } catch (e) {
                console.log(e);
            }
        },
        [addEntity, groupSelected]
    );

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
};

EntityDragAndImport.propTypes = {
    group: PropTypes.string,
    project: PropTypes.string,
    user: PropTypes.object,
    addEntity: PropTypes.func.isRequired,
    addPlaceHolderEntity: PropTypes.func.isRequired
};

const mapStateToProps = state => ({
    group: state.entities.group,
    groupSelected: state.entities.groupSelected,
    project: state.auth.userdata.project,
    user: state.auth.userdata ? state.auth.userdata.user : null
});

export default connect(
    mapStateToProps,
    {addEntity, addPlaceHolderEntity}
)(EntityDragAndImport);
