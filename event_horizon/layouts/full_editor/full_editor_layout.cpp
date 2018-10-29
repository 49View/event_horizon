//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"

#include <render_scene_graph/ui_presenter.hpp>
#include <converters/stl/parse_stl.h>
#include <converters/gltf2/gltf2.h>
#include "core/tar_util.h"
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"
#include "timeline_layout.h"
#include "console_layout.h"
#include "geom_layout.h"
#include "material_layout.h"
#include "image_layout.h"
#include "camera_layout.h"
#include "cloud_entities_layout.h"
#include "cloud_geom_layout.h"
#include "cloud_material_layout.h"

struct UIViewLayout {
    float consoleHeight = 0.0f;
    float rightPanelWidth = 0.0f;
    float leftPanelHeight = 0.0f;
    float leftPanelHeight2 = 0.0f;
    float rightPanelHeight = 0.0f;
    Vector2f main3dWindowSize = Vector2f::ZERO;
    Vector2f timeLinePanelSize = Vector2f::ZERO;
    Rect2f foxLayout;
};

std::shared_ptr<MaterialBuilder> mb;

UIViewLayout uivl;

void materialPBRCallback( const rapidjson::Document& data ) {
    std::string filename = data["name"].GetString();
    if ( filename.find(MDaemonPaths::StoredOutput) != std::string::npos ){
        FM::readRemoteSimpleCallback( filename, [&](const Http::Result& _res) {
            auto inflatedData = zlibUtil::inflateFromMemory( uint8_p{std::move(_res.buffer), _res.length} );
            auto fn = getFileNameOnly(filename);
            mb = std::make_shared<MaterialBuilder>(fn);
            auto files = tarUtil::untar( inflatedData );
            for ( const auto& fi  : files ) {
                if ( const auto r = MPBRTextures::findTextureInString(fi.name); !r.empty() ) {
                    mb->buffer( r, fi.dataPtr );
                }
            }
            UiPresenter::sUpdateCallbacks.emplace_back( []( UiPresenter* p ) {
                mb->makeDirect( p->ML() );
                if ( !p->RR().hasTag(9300) ) {
                    GeomBuilder{ShapeType::Sphere, Vector3f::ONE}.g(9300).build( p->RSG() );
                }
                p->RR().changeMaterialOnTags( 9300, std::dynamic_pointer_cast<PBRMaterial>(p->ML().get(mb->Name())) );
            } );
        } );
    }
}

void initLayout( PresenterLayout* _layout, UiPresenter* p ) {

    uivl.consoleHeight = 0.15f;
    uivl.rightPanelWidth = 0.25f;
    uivl.rightPanelHeight = (1.0f - uivl.consoleHeight)/2.0f;
    uivl.leftPanelHeight = (1.0f - uivl.consoleHeight)/3.0f;
    uivl.leftPanelHeight2 = (1.0f - uivl.consoleHeight)/4.5f;
    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.20f };
    float topX = uivl.rightPanelWidth;

    _layout->addBox( "Console", 0.0f, 1.0f, 1.0f-uivl.consoleHeight, 1.0f, ImGuiConsoleLayout );
    _layout->addBox( "SceneGeometry", 0.0f, uivl.rightPanelWidth, 0.0f, uivl.leftPanelHeight, ImGuiGeoms );
    _layout->addBox( "SceneMaterials", 0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight, uivl.leftPanelHeight*2.0f,
                     ImGuiMaterials );

    float imageTY = uivl.leftPanelHeight*2.0f + uivl.leftPanelHeight2;
    _layout->addBox( "SceneImages", 0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight*2.0f, imageTY, ImGuiImages );

    _layout->addBox( "SceneCameras", 0.0f, uivl.rightPanelWidth, imageTY, imageTY + uivl.leftPanelHeight2,ImGuiCamera );

    float timeLineY = 1.0f-(uivl.consoleHeight+uivl.timeLinePanelSize.y());
    _layout->addBox( "SceneTimeline", uivl.rightPanelWidth, uivl.rightPanelWidth + uivl.timeLinePanelSize.x(),
                     timeLineY, timeLineY + uivl.timeLinePanelSize.y(), ImGuiTimeline );

    _layout->addBox( "CloudMaterial", 1.0f-uivl.rightPanelWidth, 1.0f, 0.0f, uivl.rightPanelHeight,
                     ImGuiCloudEntitiesMaterials );

    _layout->addBox( "CloudGeometry", 1.0f-uivl.rightPanelWidth, 1.0f, uivl.rightPanelHeight, uivl.rightPanelHeight*2,
                     ImGuiCloudEntitiesGeom );

    _layout->addBox( Name::Foxtrot,
                     topX, topX + (1.0f-uivl.rightPanelWidth*2.0f),
                     0.0f, (1.0f-(uivl.consoleHeight + uivl.timeLinePanelSize.y())), CameraControls::Fly );

    Socket::on( "cloudStorageFileUpdate", materialPBRCallback );
}

void render( UiPresenter* p ) {

}

void allConversionsDragAndDropCallback( UiPresenter* p, const std::string& _path ) {
    std::string pathSanitized = url_encode_spacesonly(_path);
    std::string ext = getFileNameExt( pathSanitized );
    std::string extl = toLower(ext);
    std::string finalPath = pathSanitized;
    bool isGeom = false;

    if ( extl == ".fbx" ) {
        // Convert to GLTF
        std::string cmd = "cd " + getFileNamePath(pathSanitized) + "\n FBX2glTF " + pathSanitized;
        std::system( cmd.c_str() );
        finalPath = getFileNamePath(pathSanitized) + "/" + getFileNameOnly(finalPath) + "_out/" +
                getFileNameOnly(finalPath) + ".gltf";
        isGeom = true;
    } else if ( extl == ".obj" ) {
        // Convert to GLTF
        finalPath = getFileNamePath(pathSanitized) + "/" + getFileNameOnly(finalPath) + ".gltf";
        std::string cmd = "cd " + getFileNamePath(pathSanitized) + "\n obj2gltf -i " + pathSanitized +
                          " -o " + finalPath;
        std::system( cmd.c_str() );
        isGeom = true;
    }
    else if ( extl == ".stl" ) {
        stl::parse_stl(pathSanitized);
        isGeom = true;
    } else if ( extl == ".sbsar" ) {
        FM::copyLocalToRemote( pathSanitized, MDaemonPaths::UploadDir + getFileName(pathSanitized) );
    }
    else if ( extl == ".gltf") {
        isGeom = true;
    }

    if ( isGeom ) {
        static float ni = 0.0f;
        GLTF2 newObject{ finalPath };
        auto hierScene = newObject.convert();
        p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d(), {0.35f, 0.5f, 0.0f}, {0.0f, 0.25f, 0.0f});
        hierScene->updateTransform( Vector3f::X_AXIS * ni );
        static auto gbt = std::make_shared<GeomBuilder>( hierScene, newObject.Materials() );
        gbt->build(p->RSG());
        p->takeScreenShot( hierScene->BBox3d(), gbt->Thumb() );
        ni+=1.0f;
    }
}

std::shared_ptr<PresenterLayout> fullEditor() {
    return std::make_shared<PresenterLayout>(initLayout, render, allConversionsDragAndDropCallback);
}
