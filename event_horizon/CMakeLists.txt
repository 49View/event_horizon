cmake_minimum_required (VERSION 2.6)
include( ../CMakeLists.txt NO_POLICY_SCOPE)
project(shared)

build_core_lib( core )
build_core_lib( graphics )
build_core_lib( poly )
build_core_lib( render_scene_graph )
build_core_lib( osm )

string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Emscripten isEmscripten)
if (NOT isEmscripten)
    build_core_lib( opencvutils )
    build_core_lib( machine_learning )
    build_core_lib( ocr )
endif ()


