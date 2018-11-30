cmake_minimum_required(VERSION 3.3)

set_property( GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE )

macro(set_platform)
    message(STATUS "CMAKE SYSTEM NANME ${CMAKE_SYSTEM_NAME}")
    string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Emscripten isEmscripten)
    if (isEmscripten)
        SET(EMSCRIPTEN TRUE)
    endif ()

    string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Windows isWindows)
    if (isWindows)
        SET(WIN32 TRUE)
    endif ()

    string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Linux isLinux)
    if (isLinux)
        SET(LINUX TRUE)
    endif ()

    string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Darwin isApple)
    if (isApple)
        SET(APPLE TRUE)
    endif ()

    message(STATUS "WIN32: ${WIN32} APPLE: ${APPLE} LINUX: ${LINUX} EMSCRIPTEN: ${EMSCRIPTEN}")

    if (EMSCRIPTEN)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
        set(OPENGL_ES TRUE)
    endif ()

endmacro()

macro(register_link LIBNAME )
    list(APPEND LIBS ${LIBNAME})
endmacro(register_link)

macro(install_header_dir DIR)
    file(GLOB HEADER_FILESS "${DIR}/*.h" "${DIR}/*.hpp")
    install ( FILES ${HEADER_FILESS} DESTINATION include/${INCLUDE_INSTALL_NAME}/${DIR} )
endmacro(install_header_dir)

macro(install_headers)
    #install_header_dir( . )
    FOREACH (DIR ${HEADERS})
        install_header_dir(${DIR})
    ENDFOREACH ()

endmacro(install_headers)

macro(register_module FOLDER_NAME )
    file(GLOB_RECURSE SRC_LIST "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.h" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.hpp" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.cpp" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.cc" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.c")
    source_group("${FOLDER_NAME}" FILES ${SRC_LIST})
    message(STATUS "module name: ${MODULE_ROOT_COMP}${FOLDER_NAME}")

    list(APPEND BUILD_FILES ${SRC_LIST})

    list(APPEND HEADERS ${MODULE_ROOT_COMP}${FOLDER_NAME})
endmacro(register_module)

macro(register_module_norec FOLDER_NAME)
    file(GLOB SRC_LIST "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.h" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.hpp" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.cpp" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.cc" "${MODULE_ROOT_COMP}${FOLDER_NAME}/*.c")
    source_group("${FOLDER_NAME}" FILES ${SRC_LIST})

    list(APPEND BUILD_FILES ${SRC_LIST})
    list(APPEND HEADERS ${MODULE_ROOT_COMP}/${FOLDER_NAME})
endmacro(register_module_norec)

macro(register_platform_module_files PLATFORM_FOLDER)

    register_module_norec(${PLATFORM_FOLDER})
    if (WIN32)
        register_module_norec("${PLATFORM_FOLDER}/desktop")
        register_module("${PLATFORM_FOLDER}/desktop/windows")
        include_directories(${PLATFORM_FOLDER}/desktop/windows)
    elseif (APPLE)
        register_module_norec("${PLATFORM_FOLDER}/desktop")
        register_module("${PLATFORM_FOLDER}/desktop/apple")
        include_directories(${PLATFORM_FOLDER}/desktop/apple)
    elseif (EMSCRIPTEN)
        register_module("${PLATFORM_FOLDER}/em")
        include_directories(${PLATFORM_FOLDER}/em)
    elseif (UNIX)
        register_module_norec("${PLATFORM_FOLDER}/desktop")
        register_module("${PLATFORM_FOLDER}/desktop/linux")
        include_directories(${PLATFORM_FOLDER}/desktop/linux)
    endif ()

endmacro(register_platform_module_files)

macro(precompile_step)
    set_cpp_flags()
endmacro(precompile_step)

macro(register_core_module)

    register_platform_module_files("platform")
    register_module("descriptors")
    register_module("configuration")
    register_module("http")
    register_module("math")
    register_module("stb")
    register_module("suncalc")

    register_boost_module()
    register_socket_module()
    register_curl_module()

    if (NOT BUILD_FROM_SOURCE)
        add_lib( core )
    endif()
    add_lib( core_libs )

endmacro(register_core_module)

macro(register_graphics_module)

    register_module("ui")
    register_module("imgui")
    include_directories(${MODULE_ROOT_COMP}imgui)

    register_platform_module_files("opengl")

    if (OPENGL_ES)
        register_module_norec("opengl/es")
    else ()
        register_module_norec("opengl/desktop")
    endif ()

    if (NOT BUILD_FROM_SOURCE)
        add_lib( graphics )
    endif()
    add_lib( graphics_libs )

endmacro(register_graphics_module)

macro(register_poly_module)

    register_module("converters")
    register_module("polyclipping")
    register_module("poly2tri")
    register_module("lightmapper")
    register_module("thekla_atlas")

    list( APPEND HEADERS "poly2tri/common" "poly2tri/sweep" )
    include_directories(${MODULE_ROOT_COMP}thekla_atlas)

    if (NOT BUILD_FROM_SOURCE)
        add_lib( poly )
    endif()
    add_lib( poly_libs )

endmacro(register_poly_module)

macro(register_render_scene_graph_module)

    register_module("ui")
    register_module("layouts")
    register_platform_module_files("platform")

    if (NOT BUILD_FROM_SOURCE)
        add_lib( render_scene_graph )
    endif()

endmacro(register_render_scene_graph_module)

macro(register_opencv_module)
    list(APPEND LIBS "opencv" "${ARGN}")

    if (NOT BUILD_FROM_SOURCE)
        add_lib( opencv )
    endif()
    add_lib( opencv_libs )

endmacro(register_opencv_module)

macro(register_sh_module)

    register_module(sh)
    register_module(ray_tracing)
    register_module(mkl)

    list(APPEND LIBS "embree" "mkl" "${ARGN}")

endmacro(register_sh_module)

#macro(register_server_services_module)
#
#    register_platform_module_files("platform")
#
#    register_boost_module()
#    register_socket_module()
#    register_curl_module()
#
#    if (NOT BUILD_FROM_SOURCE)
#        add_lib( server_services )
#    endif()
#    add_lib( server_services_libs )
#
#endmacro(register_server_services_module)

macro(register_boost_module)

    list(APPEND LIBS "boost" "${ARGN}")

endmacro(register_boost_module)

macro(register_socket_module)
    list(APPEND LIBS "boost" "socket.io-client-cpp" "${ARGN}")
endmacro(register_socket_module)

macro(register_machine_learning_module)
    if (NOT BUILD_FROM_SOURCE)
        add_lib( machine_learning )
    endif()
    add_lib( machine_learning_libs )

    list(APPEND LIBS "opencv" "${ARGN}")
endmacro(register_machine_learning_module)

macro(register_ocr_module)
    if (NOT BUILD_FROM_SOURCE)
        add_lib( ocr )
    endif()
    add_lib( ocr_libs )

    list(APPEND LIBS "tesseract" "opencv" "${ARGN}")
endmacro(register_ocr_module)

macro(register_osm_module)
    if (NOT BUILD_FROM_SOURCE)
        add_lib( osm )
    endif()
endmacro(register_osm_module)

macro(register_fbx_module)

    set( MODULE_ROOT_COMP ${MODULE_ROOT})
    register_module("/converters/fbx")

    list(APPEND LIBS "fbxsdk" "${ARGN}")

endmacro(register_fbx_module)

macro(register_curl_module)

    list(APPEND LIBS "curlpp" "${ARGN}")

endmacro(register_curl_module)

macro(register_converters_module)

    set( MODULE_ROOT_COMP ${MODULE_ROOT})
    register_module("converters")

endmacro(register_converters_module)

macro(add_lib)
    set(modules ${ARGN})
    FOREACH (module ${modules})
        MESSAGE( STATUS  "adding lib ${module}" )
        register_link( ${module} )
    ENDFOREACH ()

endmacro(add_lib)

macro(compile)

    include_directories(${PROJECT_SOURCE_DIR}/..)
    include_directories(${PROJECT_SOURCE_DIR}/../..)
    include_directories(${PROJECT_SOURCE_DIR}/../event_horizon)
    include_directories(${PROJECT_SOURCE_DIR}/../../event_horizon)
    include_directories(${PROJECT_SOURCE_DIR}/../../event_horizon/event_horizon)
    if (BUILD_FROM_SOURCE)
        if (EXISTS "${PROJECT_SOURCE_DIR}/../../event_horizon/event_horizon/")
            set(MODULE_ROOT "${PROJECT_SOURCE_DIR}/../../event_horizon/event_horizon/")
        elseif( EXISTS "${PROJECT_SOURCE_DIR}/../../event_horizon/" )
            set(MODULE_ROOT "${PROJECT_SOURCE_DIR}/../../event_horizon/")
        endif()
    else()
        set( MODULE_ROOT "")
        set( MODULE_ROOT_COMP "")
    endif()

    precompile_step()

    if(EXISTS "${CMAKE_SOURCE_DIR}/main.cpp")
        register_module_norec(.)
    endif()

    set(modules ${ARGN})
    FOREACH (module ${modules})
        if(BUILD_FROM_SOURCE)
            set( MODULE_ROOT_COMP ${MODULE_ROOT}${module}/)
        else()
            set( MODULE_ROOT_COMP "${module}/")
        endif()
        register_module_norec(.)
        message(STATUS "adding ${module} to compiler")
        if (${module} STREQUAL "core")
            register_core_module()
        elseif (${module} STREQUAL "graphics")
            register_graphics_module()
        elseif (${module} STREQUAL "poly")
            register_poly_module()
        elseif (${module} STREQUAL "render_scene_graph")
            register_render_scene_graph_module()
        elseif (${module} STREQUAL "machine_learning")
            register_machine_learning_module()
        elseif (${module} STREQUAL "opencvutils")
            register_opencv_module()
        elseif (${module} STREQUAL "ocr")
            register_ocr_module()
        elseif (${module} STREQUAL "osm")
            register_osm_module()
        elseif (${module} STREQUAL "fbx")
            register_fbx_module()
        elseif (${module} STREQUAL "converters")
            register_converters_module()
        elseif (${module} STREQUAL "sh")
            register_sh_module()
        elseif (${module} STREQUAL "boost")
            register_boost_module()
        endif ()
    ENDFOREACH ()

endmacro(compile)


macro(compile_presenters)

    set(presenters ${ARGN})
    FOREACH (presenter ${presenters})
        message(STATUS "adding ${presenter} to compiler")
        #        list( APPEND BUILD_FILES "../../presenters/${presenter}.cpp" )
        #        list( APPEND BUILD_FILES "../../presenters/${presenter}.hpp" )
        list(APPEND BUILD_FILES "${presenter}")
    ENDFOREACH ()

endmacro(compile_presenters)

macro(compile_module module)

    set(MODULES_ROOT "../../")
    set(MODULE_ROOT_COMP "")
    message(STATUS "adding module ${MODULES_ROOT}${module} to compiler")
    register_module(${MODULES_ROOT}${module})

    list(APPEND LIBS "${ARGN}")

endmacro(compile_module)

macro(compile_local module)

    message(STATUS "adding folder ${module} to compiler")
    register_module(${module})

endmacro(compile_local)

macro(link_aws_s3)
    if ("s3" IN_LIST WHICH_LIBS)
        SET(S3_LIBS aws-cpp-sdk-s3 aws-cpp-sdk-core)

        link_libraries("aws" "${S3_LIBS}" "SINGLE" "${S3_LIBS}" "${S3_LIBS}" "${S3_LIBS}" OFF)

        link_system_libraries_if("${WIN32}" "winhttp;Userenv;wininet;Bcrypt;Version")
        link_system_libraries_if("${APPLE}" "-lcurl")
        link_system_libraries_if("${LINUX}" "-lcurl")
    endif ()

endmacro(link_aws_s3)

macro(link_core)
    if ("core" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewcore" "49viewcore" "49viewcore" "49viewcore" "49viewcore")
    endif ()
    if ("core" IN_LIST WHICH_LIBS OR "core_libs" IN_LIST WHICH_LIBS)
        link_restbed()
        link_sockets()
        link_curl()
        link_boost()
        if ( WIN32 )
            link_libraries_multi("libssl" "ssl" OFF OFF OFF)
            link_libraries_multi("libcrypto" "crypto" OFF OFF OFF)
            link_system_libraries_if("${WIN32}" "-lcrypt32")
        endif()
    endif ()

endmacro(link_core)

macro(link_core_graphics)
    if ("graphics" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewgraphics" "49viewgraphics" "49viewgraphics" "49viewgraphics" "49viewgraphics")
    endif ()
    if ("graphics" IN_LIST WHICH_LIBS OR "graphics_libs" IN_LIST WHICH_LIBS)
        link_graphics()
    endif ()

endmacro(link_core_graphics)

macro(link_embree)
    # embree
    if ("embree" IN_LIST WHICH_LIBS)
        include_directories(../../libs/embree/include)
        if (WIN32)
            link_libraries("embree" "embree" OFF OFF OFF OFF "embree;tbb;tbbmalloc")
        elseif (LINUX)
            #NOT needed if embree is install already (check -lembree down below)
            find_library(PATHS_embree NAME embree3 )
            target_link_libraries(${PROJECT_NAME} ${PATHS_embree})

            #if(PATHS_embree)
            #	MESSAGE(STATUS "	Embree found in " ${PATHS_embree})
            #else()
            #	MESSAGE(FATAL_ERROR "	Embree not found !!")
            #endif()
        else ()
            #NOT needed if embree is install already (check -lembree down below)
            find_library(PATHS_embree NAME embree HINTS "/opt/local/lib")
            if (PATHS_embree)
                MESSAGE(STATUS "	Embree found in " ${PATHS_embree})
            else ()
                MESSAGE(FATAL_ERROR "	Embree not found !!")
            endif ()
            target_link_libraries(${PROJECT_NAME} ${PATHS_embree})
            #link_libraries("embree" "embree" OFF OFF OFF "embree;tbb;tbbmalloc")
            MESSAGE(STATUS "	Embree loaded")
        endif ()
    endif ()
endmacro(link_embree)

macro(link_mkl)
    # mkl
    if ("mkl" IN_LIST WHICH_LIBS)
        if (WIN32)
            SET(Intel_Tool_dir "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2017.1.143/windows/mkl/")
            SET(Mkl_Lib_subfolder "intel64_win")
        elseif (LINUX)
            SET(Intel_Tool_dir "/opt/intel/compilers_and_libraries_2017.2.174/linux/mkl/")
            SET(Mkl_Lib_subfolder "intel64_lin")
        else ()
            SET(Intel_Tool_dir "/opt/intel/compilers_and_libraries_2018.0.104/mac/mkl/")
            SET(Mkl_Lib_subfolder ".")
        endif ()

        MESSAGE(STATUS "Linking MKL, searching in ${Intel_Tool_dir}")

        find_library(PATHS_mkl_core NAME mkl_core PATHS "${Intel_Tool_dir}/lib/${Mkl_Lib_subfolder}/")
        find_library(PATHS_mkl_sequential NAME mkl_sequential PATHS "${Intel_Tool_dir}/lib/${Mkl_Lib_subfolder}/")
        find_library(PATHS_mkl_ilp NAME mkl_intel_ilp64 PATHS "${Intel_Tool_dir}/lib/${Mkl_Lib_subfolder}/")

        if (PATHS_mkl_core)
            MESSAGE(STATUS "	MKL found in " ${PATHS_mkl_core})
            include_directories(${Intel_Tool_dir}/include)
            target_link_libraries(${PROJECT_NAME} ${PATHS_mkl_core})
            target_link_libraries(${PROJECT_NAME} ${PATHS_mkl_sequential})
            target_link_libraries(${PROJECT_NAME} ${PATHS_mkl_ilp})
        else ()
            MESSAGE(FATAL_ERROR "	MKL not found !!")
        endif ()
    endif ()
endmacro(link_mkl)

macro(link_fbxsdk)
    if ("fbxsdk" IN_LIST WHICH_LIBS)
        link_libraries_multi("fbxsdk" "fbxsdk" "fbxsdk" "fbxsdk" OFF)
        include_directories(../../libs/fbxsdk/include)
    endif ()
endmacro(link_fbxsdk)

macro(link_mongodbcxx)
    if ("mongodb" IN_LIST WHICH_LIBS)
        find_package(libmongocxx)
        target_link_libraries(${PROJECT_NAME} ${LIBMONGOCXX_LIBRARIES})
        message(STATUS "Linking.. mongodb")
        message(STATUS "	    mongodb resolve in ${LIBMONGOCXX_LIBRARIES}")

        include_directories(${LIBBSONCXX_INCLUDE_DIRS})
        include_directories(${LIBMONGOCXX_INCLUDE_DIRS})
    endif ()
endmacro(link_mongodbcxx)

macro(link_efsw)
    if ("efsw" IN_LIST WHICH_LIBS)
        link_libraries_multi("efsw" "efsw" "efsw" "efsw" OFF)
        include_directories(../../libs/efsw/include)
    endif ()
endmacro(link_efsw)

macro(link_boost)
    link_libraries_if("${WIN32}" "boost" "c:/usr/local/lib/" "boost_system")
    link_libraries_if("${APPLE}" "boost" "/usr/local/lib/" "boost_system")
    link_libraries_if("${LINUX}" "boost" "/usr/local/lib/" "boost_system")
endmacro(link_boost)

macro(link_sockets)
    link_libraries_multi("socket.io-client-cpp" "sioclient_tls" "sioclient_tls" "sioclient_tls" OFF)
endmacro(link_sockets)

macro(link_ffmpeg)
    # ffmpeg
    if ("ffmpeg" IN_LIST WHICH_LIBS)
        include_directories(../../libs/ffmpeg/include)
        SET(FFMPEG_LIBS swscale avdevice avformat avcodec avutil avfilter swresample)
        SET(FFMPEG_LIBS_LINUX swscale avdevice avformat avcodec avutil postproc swresample fdk-aac mp3lame vorbisenc vorbis ogg vpx x264 x265)
        SET(FFMPEG_WIN_DLL avcodec-57 avdevice-57 avfilter-6 avformat-57 avutil-55 postproc-54 swresample-2 swscale-4)
        link_libraries("ffmpeg" "${FFMPEG_LIBS}" OFF "${FFMPEG_LIBS}" "${FFMPEG_LIBS_LINUX}" "${FFMPEG_LIBS_LINUX}" "${FFMPEG_WIN_DLL}")
    endif ()
endmacro(link_ffmpeg)

macro(link_graphics)
    # glew and glfw
    if (NOT EMSCRIPTEN)
        link_libraries_multi("glew" "glew" "GLEW" OFF OFF )

        link_libraries_multi("glfw" "glfw3" "glfw3" "glfw3" "glfw3")
#        add_postbuild_copy_command("glfw" "glfw3")
        # libraries for glfw on linux; if not found then need to install with apt-get (see dockerfile script)
        link_system_libraries_if("${LINUX}" "-lXrender;-ldrm;-lXdamage;-lXext;-lX11;-pthread;-lXi;-lXrandr;-lXxf86vm;-lXinerama;-lXcursor;-lGLEW")

        # openGL
        link_package("OpenGL")

        #openvr
        #target_link_libraries (${PROJECT_NAME} debug "../../libs/oculus/LibOVR/Lib/Windows/x64/Debug/VS2015/LibOVR" optimized "../../libs/oculus/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR")
        #target_link_libraries (${PROJECT_NAME} "../../libs/oculus/LibOVRPlatform/Lib/Win/LibOVRPlatform64_1")
        #target_link_libraries (${PROJECT_NAME} "../../libs/oculus/LibOVRAvatar/Lib/Win/LibOVRAvatar")
        #openvr
#        link_libraries("openvr" "openvr_api" "SINGLE" "OpenVR" "openvr_api" OFF "openvr_api")
        link_libraries_multi("openvr" "openvr_api" OFF OFF OFF )
        # add_postbuild_copy_command("openvr" "libopenvr_api")
#        if (APPLE)
#            link_frameworks("OpenVR")
#        endif()
    endif ()

endmacro(link_graphics)

macro(link_core_poly)
    if ("poly" IN_LIST WHICH_LIBS)
#        link_polyclipper()
        link_libraries_multi("49viewpoly" "49viewpoly" "49viewpoly" "49viewpoly" "49viewpoly")
    endif()
    if ("poly" IN_LIST WHICH_LIBS OR "poly_libs" IN_LIST WHICH_LIBS)
        link_libraries_multi("thekla_atlas" "thekla_atlas" "thekla_atlas" "thekla_atlas" OFF)
        link_libraries_multi("nvmesh" "nvmesh" "nvmesh" "nvmesh" OFF)
        link_libraries_multi("nvimage" "nvimage" "nvimage" "nvimage" OFF)
        link_libraries_multi("nvmath" "nvmath" "nvmath" "nvmath" OFF)
        link_libraries_multi("nvcore" "nvcore" "nvcore" "nvcore" OFF)
    endif()

endmacro(link_core_poly)


macro(link_core_render_scene_graph)
    if ("render_scene_graph" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewrender_scene_graph" "49viewrender_scene_graph" "49viewrender_scene_graph" "49viewrender_scene_graph" "49viewrender_scene_graph")
    endif()
endmacro(link_core_render_scene_graph)

macro(link_core_osm)
    if ("osm" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewosm" "49viewosm" "49viewosm" "49viewosm" "49viewosm")
    endif()
endmacro(link_core_osm)

#macro(link_core_server_services)
#    if ("server_services" IN_LIST WHICH_LIBS)
#        link_libraries_multi("49viewserver_services" "49viewserver_services" "49viewserver_services" "49viewserver_services" "49viewserver_services")
#    endif()
#    if ( "server_services" IN_LIST WHICH_LIBS OR "server_services_libs" IN_LIST WHICH_LIBS)
#        link_boost()
#        link_sockets()
#        link_curl()
#    endif()
#
#endmacro(link_core_server_services)

macro(link_core_ocr)
    if ("ocr" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewopencvutils" "49viewopencvutils" "49viewopencvutils" "49viewopencvutils" OFF)
        link_libraries_multi("49viewocr" "49viewocr" "49viewocr" "49viewocr" OFF)
    endif()
    if ( "ocr" IN_LIST WHICH_LIBS OR "ocr_libs" IN_LIST WHICH_LIBS)
        link_opencv()
        link_tesseract()
    endif()
endmacro(link_core_ocr)

macro(link_core_machine_learning)
    if ("machine_learning" IN_LIST WHICH_LIBS)
        link_libraries_multi("49viewopencvutils" "49viewopencvutils" "49viewopencvutils" "49viewopencvutils" OFF)
        link_libraries_multi("49viewmachine_learning" "49viewmachine_learning" "49viewmachine_learning" "49viewmachine_learning" OFF)
    endif()
    if ( "machine_learning" IN_LIST WHICH_LIBS OR "machine_learning_libs" IN_LIST WHICH_LIBS)
        link_opencv()
    endif()
endmacro(link_core_machine_learning)

macro(link_leap)
    # custom for windows
    if (WIN32)
        #leap
        if ("leap" IN_LIST WHICH_LIBS)
            target_link_libraries(${PROJECT_NAME} ../../libs/LeapSDK/lib/win/leap)
            add_postbuild_copy_command("LeapSDK" "Leap")
        endif ()
    endif ()

endmacro(link_leap)

macro(link_opencv)
    if ( "opencv" IN_LIST WHICH_LIBS )

        SET(OpenCV_LIBS opencv_ml opencv_objdetect opencv_highgui opencv_imgcodecs opencv_imgproc opencv_core )
        SET(OpenCV_LIBS_MAC ${OpenCV_LIBS} )
        SET(OpenCV_LIBS_LINUX ${OpenCV_LIBS} )
        link_libraries_multi("opencv" "${OpenCV_LIBS}" "${OpenCV_LIBS_MAC}" "${OpenCV_LIBS_LINUX}" "${OpenCV_LIBS_LINUX}" OFF)

        if( WIN32 )
            SET(OpenCV_EXTRA_LIBS IlmImf libtiff libjpeg libpng libjasper libwebp )
            link_libraries_multi("opencvExtras" "${OpenCV_EXTRA_LIBS}" OFF OFF OFF )
        endif()
        if (LINUX OR APPLE)
            find_library(PATHS_IlmImf NAME IlmImf PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_IlmImf})
            message(STATUS "		IlmImf resolve in ${PATHS_IlmImf}")
            find_library(PATHS_ippiw NAME ippiw PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_ippiw})
            find_library(PATHS_libwebp NAME libwebp PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_libwebp})
            find_library(PATHS_ittnotify NAME ittnotify PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_ittnotify})
            find_library(PATHS_libjasper NAME libjasper PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_libjasper})
            find_library(PATHS_ippicv NAME ippicv PATHS "/usr/local/share/OpenCV/3rdparty/lib")
            target_link_libraries(${PROJECT_NAME} ${PATHS_ippicv})
        endif()

        link_system_libraries_if("${APPLE}" "-ljpeg;-ltiff;-lpng")

        # link_libraries_multi("49viewopencvutils" "49viewopencvutils" "49viewopencvutils" "49viewopencvutils" "49viewopencvutils")
        # if (WIN32)
        #     link_libraries_multi("opencv" "ippicvmt" OFF OFF OFF)
        # endif ()
    endif ()
endmacro(link_opencv)

macro(link_restbed)
    link_libraries_multi("restbed" "restbed" "restbed" "restbed" OFF)
    if (APPLE OR LINUX)
        message(STATUS "Linking system library -lcrypto;-lssl")
        target_link_libraries(${PROJECT_NAME} "-lcrypto;-lssl")
    endif()

#    add_postbuild_copy_command("restbed" "librestbed")
endmacro(link_restbed)

macro(link_gtest)
    if ("gtest" IN_LIST WHICH_LIBS)
        SET(GTEST_LIBS gtest gmock)
        link_libraries("gtest" "${GTEST_LIBS}" "MULTI" "${GTEST_LIBS}" "${GTEST_LIBS}" OFF)
    endif ()
endmacro(link_gtest)

macro(link_polyclipper)
#    if ("clipper" IN_LIST WHICH_LIBS)
#        include_directories(../../libs/clipper/include)
#        link_libraries_multi("clipper" "polyclipping" "polyclipping" "polyclipping" "polyclipping")
#    endif ()
endmacro(link_polyclipper)

macro(link_curl)
    link_libraries_multi("curlpp" "curlpp" "curlpp" "curlpp" OFF)

    if( WIN32 )
        link_libraries_multi("libcurl" "curl" OFF OFF OFF )
    endif()

    link_system_libraries_if("${APPLE}" "-lcurl")
    link_system_libraries_if("${LINUX}" "-lcurl")
endmacro(link_curl)

macro(link_tesseract)

    link_libraries_if("${APPLE}" "tesseract" "/usr/local/lib/" "tesseract")
    link_libraries_if("${APPLE}" "lept" "/usr/local/lib/" "lept")

    link_libraries_if("${LINUX}" "lept" "/usr/lib/" "lept")
    link_libraries_if("${LINUX}" "tesseract" "/usr/lib/" "tesseract")

    link_libraries_if("${WIN32}" "tesseract" "c:/usr/local/lib" "tesseract40")
    link_libraries_if("${WIN32}" "leptonica" "c:/usr/local/lib" "leptonica-1.76.0")
#    add_postbuild_copy_command("tesseract" "tesseract400")
#    add_postbuild_copy_command("leptonica" "pvt.cppan.demo.danbloomberg.leptonica-1.74.1")

endmacro(link_tesseract)

function(linker PROJECT_NAME WHICH_LIBS)

    set_linker_flags()

    if (NOT LIBRARIES_ROOT_FOLDER)
        set(LIBRARIES_ROOT_FOLDER "../../libs/" CACHE FILEPATH "Libs root folder")
        message(STATUS "Setting default library path to ${LIBRARIES_ROOT_FOLDER}")
    endif ()

    # check if library path ends with /. If not, add a /
    string(REGEX MATCH "\\/$" match ${LIBRARIES_ROOT_FOLDER})
    if (NOT match)
        SET(LIBRARIES_ROOT_FOLDER ${LIBRARIES_ROOT_FOLDER}/)
    endif ()
    message(STATUS "Using library path ${LIBRARIES_ROOT_FOLDER}")

    if ( NOT EMSCRIPTEN )
        link_system_libraries_if("${LINUX}" "-lpthread;-lz;-ltiff;-ldl;-lrt;-lm;-lboost_system;-ljpeg;-lpng")
    endif()

    link_core_poly()
    link_core_graphics()
    link_core_render_scene_graph()
#    link_core_server_services()
    link_core_ocr()
    link_core_osm()
    link_core_machine_learning()
    link_core()

    link_aws_s3()
    link_embree()
    link_mkl()
    link_fbxsdk()
    link_ffmpeg()
    link_leap()
    link_gtest()
    link_mongodbcxx()
    link_efsw()

    # zlib
    link_libraries_multi("zlib" "zlib" OFF OFF OFF)

    # common libraries
    link_system_libraries_if("${APPLE}" "-llapack;-lz;-lbz2;-liconv;")
    link_libraries_if("${APPLE}" "X11" "/usr/X11/lib/" "X11")

    link_system_libraries_if("${WIN32}" "-lws2_32")
    link_system_libraries_if("${WIN32}" "-lwsock32")

    # apple frameworks
    link_frameworks("OpenCL;AudioToolbox;Accelerate;AudioUnit;Cocoa;CoreAudio;CoreMedia;CoreFoundation;CoreGraphics;CoreServices;CoreVideo;Foundation;IOKit;Security;VideoToolbox;VideoDecodeAcceleration")

endfunction(linker)

# libName name of the root folder for the library
# winLibraries list of name for the library under Windows
# debugOption OFF = library doesnt have debug version 
#             SINGLE release and debug versions are in same folder (debug has d at the end of the name)
#             MULTI debug and release versions are in separate folder (/debug and /release)
# osxLibraries list of name for the library under MacOS
# linuxLibraries list of name for the library under Linux
function(link_libraries libName winLibraries debugOption osxLibraries linuxLibraries emccLibraries
        postBuildLibrariesToCopy)

    if (WIN32 AND debugOption)
        if (debugOption STREQUAL "SINGLE")
            link_libraries_windows(${libName} TRUE "${winLibraries}")
        else ()
            link_libraries_windows(${libName} FALSE "${winLibraries}")
        endif ()
    else ()
        link_libraries_multi(${libName} "${winLibraries}" "${osxLibraries}" "${linuxLibraries}" "${emccLibraries}")
    endif ()

    if (postBuildLibrariesToCopy)
        add_postbuild_copy_command(${libName} "${postBuildLibrariesToCopy}")
    endif ()

endfunction(link_libraries)

function(link_frameworks frameworks)
    if (APPLE)
        FOREACH (framework ${frameworks})
            message(STATUS "Linking framework ${framework}")
            target_link_libraries(${PROJECT_NAME} "-framework ${framework}")
        ENDFOREACH ()
    endif ()
endfunction(link_frameworks)

function(link_package packageName)
    link_package_with_version(${packageName} "")
endfunction(link_package)

function(link_package_with_version packageName version)
    message(STATUS "Linking package ${packageName} ${version}")
    find_package(${packageName} ${version})

    string(TOUPPER ${packageName} packagePrefix)

    message(STATUS "	Found ${${packagePrefix}_FOUND}")
    message(STATUS "	Version ${${packagePrefix}_VERSION}")
    message(STATUS "	Include folder ${${packagePrefix}_INCLUDE_DIR}")
    message(STATUS "	Include folders ${${packagePrefix}_INCLUDE_DIRS}")
    message(STATUS "	Library ${${packagePrefix}_LIBRARY}")
    message(STATUS "	Libraries ${${packagePrefix}_LIBRARIES}")

    include_directories(${${packagePrefix}_INCLUDE_DIR})
    include_directories(${${packagePrefix}_INCLUDE_DIRS})

    target_link_libraries(${PROJECT_NAME} ${${packagePrefix}_LIBRARY})
    target_link_libraries(${PROJECT_NAME} ${${packagePrefix}_LIBRARIES})

endfunction(link_package_with_version)

function(link_libraries_windows libName allInOneFolder libraries)
    if (WIN32)
        message(STATUS "Linking.. " "${libName}")

            # SET(libPath "${LIBRARIES_ROOT_FOLDER}${libName}/lib/win/")

            message(STATUS "	Searching in " ${libPath})

            FOREACH (lib ${libraries})
                find_library(PATHS_${lib} NAMES ${lib} PATHS ${libPath})
                target_link_libraries(${PROJECT_NAME} ${PATHS_${lib}})
                message(STATUS "		Found ${lib} in ${PATHS_${lib}}")
				if ( NOT allInOneFolder)
					find_library(PATHS_${lib}_debug NAMES ${lib}d PATHS ${libPath})
					target_link_libraries(${PROJECT_NAME} debug ${PATHS_${lib}_debug})
					message(STATUS "		Found ${lib}d debug in ${PATHS_${lib}d}")
				endif()
            ENDFOREACH ()
    endif ()
endfunction(link_libraries_windows)

# libName name of the root folder for the library
# winLibraries list of name for the library under Windows
# osxLibraries list of name for the library under MacOS
# linuxLibraries list of name for the library under Linux
# emccLibraries list of name for the library under Linux
function(link_libraries_multi libName winLibraries osxLibraries linuxLibraries emccLibraries)

    set(platformCompiler "")
    set(postfix "")
    if (WIN32)
        SET(libPath "c:/usr/local/lib")
    else ()
        SET(libPath "/usr/local/lib")
    endif()

    if (WIN32)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set(platformCompiler "MSVC/")
            SET(libPath "c:/windows/system32")
        endif()
        set(libraries ${winLibraries})
        set(archPath "win")
    elseif (APPLE)
        set(libraries ${osxLibraries})
        set(archPath "osx")
    elseif (EMSCRIPTEN)
        set(libraries ${emccLibraries})
        set(archPath "em")
        set(postfix "_em")
    elseif (UNIX)
        set(libraries ${linuxLibraries})
        set(archPath "linux")
    endif ()

    if (libraries)
        message(STATUS "Linking.. ${libName}")

        message(STATUS "	Searching multi in " ${libPath})

        FOREACH (lib ${libraries})
            set( libcomplete ${lib}${postfix} )
            message(STATUS "		Searching ${libcomplete} as ${libcomplete} in ${libPath}")
            find_library(PATHS_${libcomplete} NAMES ${libcomplete} PATHS ${libPath})
            target_link_libraries(${PROJECT_NAME} ${PATHS_${libcomplete}})
            message(STATUS "		${libcomplete} resolve in ${PATHS_${libcomplete}}")
        ENDFOREACH ()

    endif ()

endfunction(link_libraries_multi)

# FOLDER_PATH example LeapSDK/lib/x64
# LIBRARIES list of library filenames
# HANDLE_DIFFERENT_CONFIGURATION add a "d" char to the end of the library name (before ".dll")
function(add_postbuild_copy_command libName libraries)
    if (WIN32)
        SET(libPath "${PROJECT_SOURCE_DIR}/${LIBRARIES_ROOT_FOLDER}${libName}/lib/win/dll/")

        FOREACH (lib ${libraries})
            SET(libFullPath ${libPath}${lib}.dll)

            message(STATUS "Add postbuild copy command of ${libFullPath} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${lib}.dll")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${libFullPath} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${lib}.dll)
        ENDFOREACH ()
    endif ()
endfunction(add_postbuild_copy_command)

function(link_libraries_if condition libName librariesPath libraries)
    if (condition)
        message(STATUS "Linking.. ${libName}")

        message(STATUS "	Searching in " ${librariesPath})

        FOREACH (lib ${libraries})
            find_library(PATHS_${lib} NAMES ${lib} PATHS ${librariesPath})
            target_link_libraries(${PROJECT_NAME} ${PATHS_${lib}})
            message(STATUS "		Found ${lib} in ${PATHS_${lib}}")
        ENDFOREACH ()
    endif ()
endfunction(link_libraries_if)

function(link_system_libraries_if condition libraries)
    if (condition)
        FOREACH (lib ${libraries})
            message(STATUS "Linking system library ${lib}")
            target_link_libraries(${PROJECT_NAME} ${lib})
        ENDFOREACH ()
    endif ()
endfunction(link_system_libraries_if)

macro(set_cpp_flags)

    set_platform()

    if (WIN32)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            # using Clang
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(CMAKE_CXX_STANDARD 17) # C++17...
            set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
            set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
            SET(CMAKE_CXX_FLAGS "-pthread -D__STDC_FORMAT_MACROS=1 -D_WINDOWS -DWIN32 -DGLEW_STATIC -DCURL_STATICLIB -D_WIN32 -DUNICODE -std=c++17")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
            # using Intel C++
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set(CMAKE_CXX_STANDARD 17) # C++17...
            set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
            set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
            SET(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /EHsc /UMBCS /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS /MP /wd4201  /DMKL_ILP64 /std:c++17")
            add_definitions(-D__STDC_CONSTANT_MACROS)
        endif ()
    elseif (APPLE)
        set(CMAKE_CXX_STANDARD 17) # C++17...
        set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
        set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
        add_definitions(-DOSX)
        add_definitions(-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)
        add_definitions(-D__STDC_CONSTANT_MACROS)
        SET(CMAKE_CXX_FLAGS "-Wall -Werror")
    elseif (UNIX)
        if (EMSCRIPTEN)
            set(CMAKE_CXX_STANDARD 17) # C++17...
            set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
            set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
            add_definitions(-DEMSCRIPTEN)
            add_definitions(-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)
            add_definitions(-D__STDC_CONSTANT_MACROS)
            if (OPENGL_ES)
                add_definitions(-D_OPENGL_ES=3)
            endif ()
            SET(CMAKE_CXX_FLAGS "-Wall -Werror --bind")
        else ()
            add_definitions(-DLINUX)
            SET(CMAKE_CXX_FLAGS "-D__STDC_FORMAT_MACROS=1 -DUNICODE -std=c++17 -Wall -Werror")
            add_definitions(-D__STDC_FORMAT_MACROS=1)
        endif ()
    endif ()

    set(BUILD_SHARED_LIBS OFF)

    add_definitions(-D_OPENGL)
    add_definitions(-DSIXTHVIEW)
    if ( USE_LOCALHOST )
        add_definitions(-DUSE_LOCALHOST=1)
    endif()

    set_include_paths()

endmacro(set_cpp_flags)

macro(set_linker_flags)

    if (WIN32)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmtd.lib /ignore:4099")
            set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB:libcmt.lib")
        endif ()
    elseif (APPLE)
    elseif (EMSCRIPTEN)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_ZLIB=1 -s LINKABLE=1 -s SOCKET_DEBUG=1")
    elseif (UNIX)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--whole-file")
    endif ()

endmacro(set_linker_flags)

macro(set_include_paths)

    if (WIN32)
        include_directories(c:/usr/local/include/)
        include_directories(c:/usr/local/include/49view)
    elseif (APPLE)
        include_directories(/usr/local/include/)
        include_directories(/usr/local/include/49view)
    elseif (EMSCRIPTEN)
        include_directories(/usr/local/include/)
        include_directories(/usr/local/include/zlib)
    elseif (UNIX)
        include_directories(/usr/include/)
        include_directories(/usr/local/include/49view)
    endif ()

#    include_directories(../../)
#    include_directories(../../libs)
#    include_directories(../../event_horizon)

endmacro(set_include_paths)

macro(build_core_lib LIBNAME)
    unset(HEADERS)
    unset(BUILD_FILES)
    set( POSTFIX_LIBNAME "")
    string(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" Emscripten isEmscripten)
    if (isEmscripten)
        set( POSTFIX_LIBNAME "_em")
    endif ()

    set(LIBNAME_INSTALL 49view${LIBNAME}${POSTFIX_LIBNAME})
    set(INCLUDE_INSTALL_NAME 49view/)

    compile( ${LIBNAME} )

    add_library(${LIBNAME_INSTALL} STATIC ${BUILD_FILES})

    install ( TARGETS ${LIBNAME_INSTALL} DESTINATION lib )
    install_headers( ${HEADERS} )

endmacro(build_core_lib)
