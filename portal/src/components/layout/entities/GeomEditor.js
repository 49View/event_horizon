import React, {Fragment} from "react";
import {useSelector} from "react-redux";
import SmallEntriesDialog from "./SmallEntriesDialog";
import {fillMaterialParams} from "utils/materialUtils";
import MaterialParametersEditor from "./MaterialParametersEditor";

const GeomEditor = () => {

    const currentEntityNodes = useSelector(state => state.entities.currentEntityNodes);
    let matSimples = [];

    // eslint-disable-next-line
    for (const e of currentEntityNodes) {
        matSimples.push(
            fillMaterialParams(e.key, e.value.values, e.value.thumbValues)
        );
    }

    const InObjectMaterials = () => {
        return (
            <Fragment>
                <div className="nodeViewer-a">
                    <div className="titleMargin">Materials</div>
                    {matSimples.map(e => (
                        <MaterialParametersEditor entity={e}/>
                    ))}
                </div>
            </Fragment>
        );
    };

    return (
        <Fragment>
            <SmallEntriesDialog/>
            <InObjectMaterials/>
        </Fragment>
    );
};

export default GeomEditor;
