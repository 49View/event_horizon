cmake_minimum_required(VERSION 2.6)
include(../../CMakeLists.txt NO_POLICY_SCOPE)
project(scripting)

set(BUILD_FROM_SOURCE TRUE)

compile( lua )
compile_local( scripting )

add_executable(${PROJECT_NAME} ${BUILD_FILES})

linker(${PROJECT_NAME} "${LIBS}")

install (TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin )
