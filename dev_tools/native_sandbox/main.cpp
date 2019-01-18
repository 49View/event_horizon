#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include "render_scene_graph/event_horizon.h"

#include <core/file_manager.h>
#include <core/math/anim.h>
#include <core/node.hpp>
#include <graphics/platform_graphics.hpp>
#include <graphics/shader_manager.h>
#include <poly/geom_builder.h>
#include <poly/polyclipping/clipper.hpp>
#include <poly/triangulator.hpp>
#include <poly/polypartition.h>
#include <poly/profile.hpp>
#include <poly/profile_builder.h>
#include <poly/ui_shape_builder.h>
#include <poly/converters/svg/svgtopoly.hpp>

std::string svgString = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<!-- Generator: Adobe Illustrator 22.1.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->\n"
                        "<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n"
                        "\t viewBox=\"0 0 1920 1080\" style=\"enable-background:new 0 0 1920 1080;\" xml:space=\"preserve\">\n"
                        "<style type=\"text/css\">\n"
                        "\t.st0{fill:#2B91F7;}\n"
                        "</style>\n"
                        "<g>\n"
                        "\t<g>\n"
                        "\t\t<polygon class=\"st0\" points=\"540.4,353.1 497.3,353.1 497.3,575.4 433.5,575.4 481.6,429.6 440.6,429.6 391.5,578.5 391.5,621 \n"
                        "\t\t\t497.3,621 497.3,749.6 540.4,749.6 540.4,621 566.7,621 566.7,575.4 540.4,575.4 \t\t\"/>\n"
                        "\t\t<path class=\"st0\" d=\"M738.4,367c-7.1-6.9-15.4-12.2-24.8-15.7c-9.5-3.5-19.6-5.3-30.4-5.3c-11.1,0-21.5,1.8-31.2,5.3\n"
                        "\t\t\tc-9.6,3.5-18,8.8-25.1,15.7c-7.1,6.9-12.7,15.2-17,24.8c-4.2,9.6-6.3,20.5-6.3,32.7V544c0,24.3,6,42.4,18,54.2\n"
                        "\t\t\tc12,11.8,26.6,17.7,43.8,17.7c9.8,0,18.8-1.7,27.1-5.1c8.3-3.4,15.8-8.1,22.5-14.2v80.5c0,11.5-3.1,20.1-9.4,25.8\n"
                        "\t\t\tc-6.3,5.7-14.8,8.6-25.6,8.6c-10.5,0-18.7-2.9-24.6-8.6c-5.9-5.7-8.9-14.3-8.9-25.8v-8.1L603.7,658v20.3\n"
                        "\t\t\tc0,24.3,6.9,43.5,20.8,57.5c13.8,14,32.6,21,56.2,21c11.8,0,22.6-1.8,32.4-5.3c9.8-3.5,18.2-8.8,25.3-15.7\n"
                        "\t\t\tc7.1-6.9,12.7-15.2,16.7-24.8c4.1-9.6,6.1-20.5,6.1-32.7V424.5c0-12.2-2-23-6.1-32.7C751.1,382.2,745.5,373.9,738.4,367z\n"
                        "\t\t\t M715.1,554.2c-5.4,6.4-11.1,11.2-17,14.4c-5.9,3.2-12.7,4.8-20.5,4.8c-11.5,0-19.5-3.5-24.1-10.4c-4.6-6.9-6.8-16.3-6.8-28.1\n"
                        "\t\t\tV425.5c0-11.5,3.2-20.1,9.6-25.8c6.4-5.7,14.7-8.6,24.8-8.6c10.1,0,18.3,2.9,24.6,8.6c6.2,5.7,9.4,14.4,9.4,25.8V554.2z\"/>\n"
                        "\t\t<polygon class=\"st0\" points=\"882.3,670.1 840.2,440.2 796.7,440.2 860,749.6 904,749.6 969.4,440.2 926.3,440.2 \t\t\"/>\n"
                        "\t\t<path class=\"st0\" d=\"M1028.3,298.6c-13.5,0-23.6,4-30.1,12.1c-6.5,8.1-9.8,18.1-9.8,30.1c0,12,3.2,22.3,9.8,30.8\n"
                        "\t\t\tc6.5,8.6,16.5,12.9,30.1,12.9c14.1,0,24.5-4.3,31.2-12.9c6.8-8.6,10.2-18.9,10.2-30.8c0-12-3.4-22-10.2-30.1\n"
                        "\t\t\tC1052.8,302.6,1042.4,298.6,1028.3,298.6z\"/>\n"
                        "\t\t<rect x=\"1006.8\" y=\"440.2\" class=\"st0\" width=\"45.1\" height=\"309.5\"/>\n"
                        "\t\t<path class=\"st0\" d=\"M1177.3,630.4c9.6-6.9,18-13.4,25.1-19.5c17.2-14.2,29.8-28.4,37.7-42.8c7.9-14.3,11.9-31.1,11.9-50.4\n"
                        "\t\t\tc0-15.5-2.3-28.4-6.8-38.7c-4.6-10.3-10.4-18.7-17.5-25.1c-7.1-6.4-15.2-11-24.3-13.7c-9.1-2.7-18.2-4-27.4-4\n"
                        "\t\t\tc-10.5,0-20.3,1.7-29.4,5.1c-9.1,3.4-17.1,8.4-23.8,14.9c-6.8,6.6-12.1,14.5-16,23.8c-3.9,9.3-5.8,19.7-5.8,31.1v167.6\n"
                        "\t\t\tc0,11.8,1.9,22.3,5.8,31.4c3.9,9.1,9.2,17,16,23.6c6.7,6.6,14.6,11.6,23.6,14.9c8.9,3.4,18.7,5.1,29.1,5.1\n"
                        "\t\t\tc10.5,0,20.3-1.7,29.4-5.1c9.1-3.4,17.1-8.4,24.1-14.9c6.9-6.6,12.3-14.4,16.2-23.6c3.9-9.1,5.8-19.6,5.8-31.4v-31.9l-41,10.6\n"
                        "\t\t\tv19.8c0,10.8-3,19-9.1,24.6c-6.1,5.6-14,8.4-23.8,8.4c-9.5,0-17.2-2.8-23.3-8.4c-6.1-5.6-9.1-13.8-9.1-24.6v-24.8\n"
                        "\t\t\tC1156.8,644.6,1167.6,637.3,1177.3,630.4z M1144.6,513.1c0-11.1,2.7-19.8,8.1-26.1c5.4-6.2,13-9.4,22.8-9.4c8.8,0,16.5,3.1,23,9.4\n"
                        "\t\t\tc6.6,6.3,9.9,17.5,9.9,33.7c0,12.5-2.2,23.5-6.6,32.9c-4.4,9.5-11.8,19.1-22.3,28.9c-8.1,7.8-19.8,16.5-34.9,26.3V513.1z\"/>\n"
                        "\t\t<polygon class=\"st0\" points=\"1460.1,659 1422.7,440.2 1381.1,440.2 1340.6,658.5 1318.3,440.2 1276.3,440.2 1314.3,749.6 \n"
                        "\t\t\t1356.8,749.6 1401.4,516.2 1442.4,749.6 1486.5,749.6 1528.5,440.2 1486,440.2 \t\t\"/>\n"
                        "\t</g>\n"
                        "\t<path class=\"st0\" d=\"M1675.4,352.8v-43.8c0-63.6-51.7-115.3-115.3-115.3H961.2h-2.4H359.9c-63.6,0-115.3,51.7-115.3,115.3v43.8\n"
                        "\t\tc-16.2,9.4-27.1,26.8-27.1,46.8v280.7c0,20,10.9,37.5,27.1,46.8v43.8c0,63.6,51.7,115.3,115.3,115.3h598.9h2.4h598.9\n"
                        "\t\tc63.6,0,115.3-51.7,115.3-115.3v-43.8c16.2-9.4,27.1-26.8,27.1-46.8V399.7C1702.5,379.6,1691.6,362.2,1675.4,352.8z M1632.3,770.9\n"
                        "\t\tc0,39.8-32.4,72.1-72.1,72.1H961.2h-2.4H359.9c-39.8,0-72.1-32.4-72.1-72.1V309.1c0-39.8,32.4-72.1,72.1-72.1h598.9h2.4h598.9\n"
                        "\t\tc39.8,0,72.1,32.4,72.1,72.1V770.9z\"/>\n"
                        "</g>\n"
                        "</svg>";

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 0.5f, CameraControls::Fly );
    _layout->addBox( SceneLayoutDefaultNames::Console, 0.0f, 1.0f, 1.0f-0.2f, 1.0f );

    _layout->addBox( SceneLayoutDefaultNames::Timeline, 0.0f, 1.0f, 1.0f-0.5f, 1.0f-0.2f );

    _p->postActivate( [](Scene* _p) {

//        ProfileBuilder pb{0.015f, 6.0f};
//        GB{GeomBuilderType::svg}.ascii(svgString).pb(ProfileBuilder{0.015f, 6.0f}).col(Color4f::AQUAMARINE).buildr(_p->RSG());

        auto c = _p->CM().getCamera(Name::Foxtrot);
        c->setPosition( Vector3f{0.0f, 1.0f, 3.0f} );

        auto cube = GB{ ShapeType::Cube }.col(Color4f::RED).at({1.66f, 0.0f, 0.0f}).buildr(_p->RSG());

//        auto pin = GB{ GeomBuilderType::file, "pin" }.buildr(_p->RSG());
//        auto text = UISB{ UIShapeType::Text3d, "Hello", 0.6f }.c(Color4f::AQUAMARINE).buildr(_p->RSG());

//        auto v = _p->RSG().Nodes();

//        LOGR( "* %s", cube->Hash().c_str() );
//        LOGR( "* %s", text->Hash().c_str() );

//        LOGR( "* %s", pin->Hash().c_str() );
//        for ( const auto& [k,pg] : v ) {
//            LOGR( "- %s", std::string(pg->Hash()).c_str() );
//        }
//        const std::string cName = "urca";
//        Timeline::add( cName, c->PosAnim(), {0.0f, Vector3f::Z_AXIS} );
//        Timeline::add( cName, c->PosAnim(), {4.0f, Vector3f::ZERO} );
//        Timeline::add( cName, c->PosAnim(), {2.0f, Vector3f::ONE} );

//        TimelineStream<V3f>{c->PosAnim()}.
//                  k(0.0f, Vector3f{0.0f, 1.0f, 0.0f}).
//                  k(5.0f, Vector3f{3.0f, 1.0f, 3.0f}).
//                  k(15.0f, V3f::Y_AXIS*1.0f).
//                  add(cName);
//        TimelineStream<V3f>{cube->PosAnim()}.
//                k(0.0f, Vector3f{0.0f, 0.0f, 0.0f}).
//                k(4.0f, Vector3f{.1f, 0.1f, 1.f}).
//                k(4.31f, Vector3f{0.0f, 1.0f, 1.0f}).
//                add(cName);

    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
