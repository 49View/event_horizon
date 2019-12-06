cmake_minimum_required (VERSION 2.6)
project(asset_daemon)

include(../../CMakeLists.txt NO_POLICY_SCOPE)

compile( core )
add_lib( mongodb )

add_executable(${PROJECT_NAME} ${BUILD_FILES})

linker(${PROJECT_NAME} "${LIBS}" )

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin )