//
// Created by Dado on 08/02/2018.
//

#include "render_orchestrator.h"
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/math/vector_util.hpp>
#include <core/raw_image.h>
#include <core/camera.h>
#include <core/node.hpp>
#include <core/geom.hpp>
#include <core/resources/material.h>
#include <graphics/renderer.h>
#include <graphics/shader_manager.h>
#include <graphics/vp_builder.hpp>
#include <graphics/window_handling.hpp>
#include <render_scene_graph/render_orchestrator_callbacks.hpp>
#include <render_scene_graph/lua_scripts.hpp>

std::vector<std::string> RenderOrchestrator::callbackPaths;
std::vector<PresenterUpdateCallbackFunc> RenderOrchestrator::sUpdateCallbacks;

Vector2i ResizeData::callbackResizeFrameBuffer = Vector2i::ZERO;
Vector2i ResizeData::callbackResizeFrameBufferOld = Vector2i::ZERO;

void RenderOrchestrator::setDragAndDropFunction( DragAndDropFunction dd ) {
    dragAndDropFunc = dd;
}

void RenderOrchestrator::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
    sUpdateCallbacks.push_back( uc );
}

void RenderOrchestrator::updateCallbacks() {

    if ( !sUpdateCallbacks.empty() ) {
        for ( auto& c : sUpdateCallbacks ) {
            c( this );
        }
        sUpdateCallbacks.clear();
    }

    if ( dragAndDropFunc ) dragAndDropFunc( callbackPaths );

    resizeCallbacks();
}

template <typename T>
void setMatProperty( std::vector<std::shared_ptr<VPList>>& vList, std::shared_ptr<RenderMaterial> mat,
                     const std::string& _uniform, const T& _value ) {
    for ( const auto& v : vList ) {
        if ( v->getMaterial() == mat ) v->getMaterial()->setConstant( _uniform, _value );
    }
}

template <typename F, typename T, typename ...Args>
void foreachCL( CommandBufferListVectorMap& CL, F func, const T& _value, Args ...args ) {
    for ( auto&[k, vl] : CL ) {
        vl.foreach( func, std::forward<Args>( args )..., _value );
    }
}

RenderOrchestrator::RenderOrchestrator( Renderer& rr, SceneGraph& _sg ) : rr( rr ), sg(_sg) {

    sg.preloadCompleteConnect( [this]( ConnectVoidParamSig _value ) {
        this->RR().setLoadingFlag(!_value );
    });

    sg.FM().connect( [](const ResourceTransfer<Font>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<Font>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.PL().connect( [](const ResourceTransfer<Profile>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<Profile>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.TL().connect( [this](const ResourceTransfer<RawImage>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<RawImage>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addTextureResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.ML().connect( [this](const ResourceTransfer<Material>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<Material>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addMaterialResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.VL().connect( [this](const ResourceTransfer<VData>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addVDataResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.GM().connect( [](const ResourceTransfer<Geom>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.nodeAddConnect( [this]( NodeGraphConnectParamsSig _geom ) {
        auto bEmpty = _geom->empty();
        LOGRS( "[SG-Node] Add " << (bEmpty ? "Root " : "") << _geom->Name() );
        if ( bEmpty ) return;
        auto dataRef = _geom->DataRef(0);
        auto vp = VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ this->RR(), dataRef.material, dataRef.vData}.
                  n(_geom->UUiD()).
                  g(_geom->Tag()).
                  t(_geom->getLocalHierTransform()).
                  b(_geom->BBox3d()).
                  build();
        this->RR().VPL( CommandBufferLimits::PBRStart, vp);
        this->RR().invalidateOnAdd();
    });

    sg.nodeRemoveConnect( [this]( NodeGraphConnectParamsSig _geom ) {
        this->RR().clearBucket( CommandBufferLimits::PBRStart );
    });

    sg.replaceMaterialConnect( [this]( const std::string& _oldMatRef , const std::string& _newMatRef ) {
        this->RR().replaceMaterial( _oldMatRef, _newMatRef );
    });

    sg.changeMaterialPropertyConnectString( [this]( const std::string& _prop, const std::string& _key,
                                                 const std::string& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
    });

    sg.changeMaterialPropertyConnectFloat( [this]( const std::string& _prop, const std::string& _key,
                                                 const float& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
    });

    sg.changeMaterialPropertyConnectV3f( [this]( const std::string& _prop, const std::string& _key,
                                              const V3f& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                this->RR().getRenderMaterialFromHash( _key ), _prop );
    });

    sg.changeMaterialPropertyConnectV4f( [this]( const std::string& _prop, const std::string& _key,
                                                 const V4f& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
    });

}

//#define LUA_HARDCODED_DEBUG

void RenderOrchestrator::luaUpdate( const AggregatedInputData& _aid ) {

#ifdef     LUA_HARDCODED_DEBUG
    lua.script( R"(

function update(aid)
  if ( aid:isMouseSingleTap(0) == true ) then
    print("Z")
    end
  end

)");
#else
    if ( auto luaScript = getLuaScriptHotReload(); !luaScript.empty() ) {
        try {
            lua.safe_script( luaScript );
        } catch (const std::exception& e) { // caught by reference to base
            std::cout << " a standard exception was caught, with message '"
                      << e.what() << "'\n";
        } catch (...) {
            std::cout << "... Except not handled";
        }

        setLuaScriptHotReload("");
    }
#endif
    // Call update every frame
    try {
        sol::protected_function updateFunction = lua["update"];
        if ( updateFunction ) {
            updateFunction(_aid);
        }
    } catch (const std::exception& e) { // caught by reference to base
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    } catch (...) {
        std::cout << "... Except not handled";
    }
}

void RenderOrchestrator::updateInputs( const AggregatedInputData& _aid ) {
    luaUpdate(_aid);
    updateCallbacks();

    for ( auto& [k,v] : mRigs ) {
        v->updateFromInputData( _aid );
    }
}

std::string boolAlphaBinary( bool _flag ) {
    return _flag ? "1" : "0";
}

void RenderOrchestrator::init() {
    initWHCallbacks();

    lua.open_libraries( sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table
            , sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::ffi);

    auto v2fLua = lua.new_usertype<Vector2f>("V2f",
                                          sol::constructors<>());
    v2fLua["x"] = &Vector2f::x;
    v2fLua["y"] = &Vector2f::y;

    auto aid = lua.new_usertype<AggregatedInputData>("AggregatedInputData",
            sol::constructors<>() );

    aid["scrollValue"] = &AggregatedInputData::scrollValue;
    aid.set("isMouseTouchedDownFirstTime", sol::readonly(&AggregatedInputData::isMouseTouchedDownFirstTime));
    aid.set("isMouseTouchedUp", sol::readonly(&AggregatedInputData::isMouseTouchedUp));
    aid.set("isMouseTouchedDown", sol::readonly(&AggregatedInputData::isMouseTouchedDown));
    aid.set("isMouseSingleTap", sol::readonly(&AggregatedInputData::isMouseSingleTap));
    aid.set("hasMouseMoved", sol::readonly(&AggregatedInputData::hasMouseMoved));

    aid.set("mousePos", sol::readonly(&AggregatedInputData::mousePos));
    aid.set("moveDiffSS", sol::readonly(&AggregatedInputData::moveDiffSS));
    aid.set("moveDiff", sol::readonly(&AggregatedInputData::moveDiff));
    aid.set("checkKeyToggleOn", sol::readonly(&AggregatedInputData::checkKeyToggleOn));

    // State machine require
    lua.require_script("statemachine", luaStateMachine);

    auto luarr = lua["rr"].get_or_create<sol::table>();

    luarr["clearColor"] = [](const std::string& _col ) {
        Renderer::clearColor( V4f::XTORGBA(_col) );
    };
    luarr["useSkybox"] = [&](bool _flag) {
        useSkybox(_flag);
    };

    luarr["useVignette"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine", "_VIGNETTING_", boolAlphaBinary(_flag) );
    };

    luarr["useFilmGrain"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                                     "_GRAINING_", boolAlphaBinary(_flag) );
    };

    luarr["useBloom"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                               "_BLOOMING_", boolAlphaBinary(_flag) );
    };

    luarr["useDOF"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                               "_DOFING_", boolAlphaBinary(_flag) );
    };

    luarr["useSSAO"] = [&](bool _flag) {
        useSSAO(_flag);
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                               "_SSAOING_", boolAlphaBinary(_flag) );
    };

#ifndef _PRODUCTION_
    Socket::on( "shaderchange",
                std::bind(&RenderOrchestrator::reloadShaders, this, std::placeholders::_1, std::placeholders::_2 ) );
#endif

    // Set a fullscreen camera by default
    addRig<CameraControlFly>( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f );
}

void RenderOrchestrator::reloadShaders( const std::string& _msg, SocketCallbackDataType&& _data ) {

    ShaderLiveUpdateMap shadersToUpdate{std::move(_data)};

	for ( const auto& ss : shadersToUpdate.shaders ) {
		rr.injectShader( ss.first, ss.second );
	}

	addUpdateCallback( [this](UpdateCallbackSign) { rr.cmdReloadShaders( {} ); } );

}

//void RenderOrchestrator::addImpl( NodeVariants _geom ) {
//}

//void RenderOrchestrator::removeImpl( const UUID& _uuid ) {
//    auto removeF = [&](const UUID& _uuid) { rr.removeFromCL(_uuid ); };
//    if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
//        std::visit( [&](auto&& arg) { arg->visitHashRecF(removeF);}, it->second );
//    }
//}
//
//void RenderOrchestrator::cmdChangeTimeImpl( const std::vector<std::string>& _params ) {
//    SB().buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
//    RR().changeTime( SB().getSunPosition() );
//}
//
//void RenderOrchestrator::cmdloadObjectImpl( const std::vector<std::string>& _params ) {
//    Vector3f pos = Vector3f::ZERO;
//    Vector3f rot = Vector3f::ZERO;
//    std::string objName = _params[0];
//    if ( _params.size() == 4 || _params.size() == 7 ) {
//        pos = { std::stof(_params[1]), std::stof(_params[2]), std::stof(_params[3]) };
//    }
//    if ( _params.size() == 7 ) {
//        rot = { std::stof(_params[4]), std::stof(_params[5]), std::stof(_params[6]) };
//    }
////    ### I'm pretty positive Geom Files need to go through the load function of a dependent geom builder.
////    GF{*this, _params[0] }.build();
//}

void RenderOrchestrator::changeMaterialTagCallback( const std::vector<std::string>& _params ) {
//    std::shared_ptr<Material> mat = std::dynamic_pointer_cast<Material>(sg.ML().get(concatParams(_params, 1)));
//    rr.changeMaterialOnTagsCallback( { sg.getGeomType( _params[0] ), mat->Name() } );
}

//void RenderOrchestrator::changeMaterialTagImpl( const std::vector<std::string>& _params ) {
//    std::string keys = concatenate(" ", { _params.begin()+1, _params.end()} );
//    MB{ML(), concatParams(_params, 1)}.load(
//            std::bind( &RenderOrchestrator::changeMaterialTagCallback, this, _params ),
//            _params );
//}

void RenderOrchestrator::changeMaterialColorCallback( const std::vector<std::string>& _params ) {
    rr.changeMaterialColorOnTags( sg.getGeomType( _params[0] ), sg.CL().get(concatParams(_params, 1))->color );
}

Renderer& RenderOrchestrator::RR() { return rr; }

void RenderOrchestrator::addBoxToViewport( const std::string& _name, const SceneScreenBox& _box ) {
    if ( boxes.find(_name) != boxes.end() ) return;
    boxes[_name] = _box;
}

std::shared_ptr<CameraRig> RenderOrchestrator::getRig( const std::string& _name ) {
    return sg.CM().get(_name);
}

std::shared_ptr<Camera> RenderOrchestrator::getCamera( const std::string& _name ) {
    return sg.CM().get(_name)->getMainCamera();
}

const Camera* RenderOrchestrator::getCamera( const std::string& _name ) const {
    return sg.CM().get(_name)->getMainCamera().get();
}

void RenderOrchestrator::setViewportOnRig( std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport ) {
    rr.getTarget(_rig->Name())->getRig()->setViewport(_viewport);
}

void RenderOrchestrator::clearPBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->clearCB();
    }
}

void RenderOrchestrator::hidePBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableBucket( false );
    }
}

void RenderOrchestrator::showPBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableBucket( true );
    }
}

void RenderOrchestrator::createSkybox( const SkyBoxInitParams& _skyboxParams ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->createSkybox( _skyboxParams );
    }
}

void RenderOrchestrator::changeTime( const std::string& _time ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->changeTime( _time );
    }
}

void RenderOrchestrator::useSkybox( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableSkybox( _value );
    }
}

void RenderOrchestrator::useSSAO( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->useSSAO( _value );
    }
}

floata& RenderOrchestrator::skyBoxDeltaInterpolation() {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        return pbrTarget->skyBoxDeltaInterpolation();
    }
    ASSERT("Target doesnt have skybox!");
    static floata reterror;
    return reterror;
}



void RenderOrchestrator::setViewportOnRig( const std::string& _rigName, const Rect2f& _viewport ) {
    rr.getTarget(_rigName)->getRig()->setViewport(_viewport);
}

void RenderOrchestrator::addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible ) {
//    if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
//        addBoxToViewport( _name,{ { _l, _r, _t, _b}, rlf->second } );
//        boxes[_name].setVisible( _bVisible );
//    }
}

void RenderOrchestrator::resizeCallback( const Vector2i& _resize ) {
    LOGR("ResizeCallbackViewports Start");
    for ( auto& [k,v] : boxes ) {
        LOGRS("Resizing " << k);
        orBitWiseFlag( v.flags, BoxFlags::Resize );
        if ( getRig(k) ) {
            LOGRS("Resizing Rig " << k);
            auto r = v.updateAndGetRect();
            rr.getTarget( k )->resize( r );
            sg.CM().get(k)->setViewport( r );
        }
    }
}

PickRayData RenderOrchestrator::rayViewportPickIntersection( const V2f& _screenPos ) const {
    return getCamera( Name::Foxtrot )->rayViewportPickIntersection( _screenPos );
}


//void RenderOrchestrator::changeMaterialColorTagImpl( const std::vector<std::string>& _params ) {
//    ColorBuilder{cl, concatParams(_params, 1)}.load(std::bind( &RenderOrchestrator::changeMaterialColorCallback,
//                                                               this,
//                                                               _params), _params);
//}
//
//void RenderOrchestrator::cmdRemoveGeometryImpl( const std::vector<std::string>& _params ) {
//    remove( _params[0] );
//}
//
//void RenderOrchestrator::updateImpl() {
//    am->update();
//}

AVInitCallback RenderOrchestrator::avcbTM() {
    return std::bind(&TextureManager::preparingStremingTexture,
                     rr.TM().get(),
                     std::placeholders::_1,
                     std::placeholders::_2);
}

void RenderOrchestrator::setVisible( uint64_t _cbIndex, bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->setVisibleCB( _cbIndex, _value );
    }
}

