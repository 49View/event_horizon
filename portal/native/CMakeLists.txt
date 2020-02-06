cmake_minimum_required (VERSION 2.6)
include(../../native/CMakeLists.txt NO_POLICY_SCOPE)
project(editor)

set(BUILD_FROM_SOURCE TRUE)
compile( core lua graphics poly render_scene_graph converters )
compile_local( editor )

add_executable(${PROJECT_NAME} ${BUILD_FILES})

linker(${PROJECT_NAME} "${LIBS}")

install (TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin )
