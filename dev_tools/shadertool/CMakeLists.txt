cmake_minimum_required (VERSION 2.6)
include(../../CMakeLists.txt NO_POLICY_SCOPE)

project(shadertool)

compile( core graphics )

add_executable(${PROJECT_NAME} ${BUILD_FILES})

link_boost()
linker(${PROJECT_NAME} "${LIBS}")

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin )

