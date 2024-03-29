set(MY_OPENCV_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/VideoCapture.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/DashboardDisplay.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/mocking.cpp")

set(MY_OPENCV_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/VideoCapture.h"
                      "${CMAKE_CURRENT_SOURCE_DIR}/DashboardDisplay.h"
                      "${CMAKE_CURRENT_SOURCE_DIR}/mocking.h")

set(MY_OPENCV_INCLUDES "./")

add_library(${PROJECT_MY_OPENCV} STATIC ${MY_OPENCV_SOURCES} ${MY_OPENCV_HEADERS})

# Threads, have CMake package configurations available
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
#locate and provide the necessary include directories
#${GTK2_INCLUDE_DIRS} and ${GTK2_LIBRARIES which for some reason were not required by
#target_include_directories and target_link_libraries
if(WIN32)
elseif(APPLE)
else()
    pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
endif()

target_link_libraries(${PROJECT_MY_OPENCV} PUBLIC Threads::Threads opencv_core opencv_highgui)

# Set OpenCV include directories
set(OpenCV_INCLUDE_DIRS
    ${OpenCV_SOURCE_DIR}/include
    ${OPENCV_MODULE_opencv_calib3d_LOCATION}/include
    ${OPENCV_MODULE_opencv_core_LOCATION}/include
    ${OPENCV_MODULE_opencv_dnn_LOCATION}/include
    ${OPENCV_MODULE_opencv_features2d_LOCATION}/include
    ${OPENCV_MODULE_opencv_flann_LOCATION}/include
    ${OPENCV_MODULE_opencv_gapi_LOCATION}/include
    ${OPENCV_MODULE_opencv_highgui_LOCATION}/include
    ${OPENCV_MODULE_opencv_imgcodecs_LOCATION}/include
    ${OPENCV_MODULE_opencv_imgproc_LOCATION}/include
    ${OPENCV_MODULE_opencv_java_LOCATION}/include
    ${OPENCV_MODULE_opencv_java_bindings_generator_LOCATION}/include
    ${OPENCV_MODULE_opencv_js_bindings_generator_LOCATION}/include
    ${OPENCV_MODULE_opencv_ml_LOCATION}/include
    ${OPENCV_MODULE_opencv_objc_bindings_generator_LOCATION}/include
    ${OPENCV_MODULE_opencv_objdetect_LOCATION}/include
    ${OPENCV_MODULE_opencv_photo_LOCATION}/include
    ${OPENCV_MODULE_opencv_python2_LOCATION}/include
    ${OPENCV_MODULE_opencv_python3_LOCATION}/include
    ${OPENCV_MODULE_opencv_python_bindings_generator_LOCATION}/include
    ${OPENCV_MODULE_opencv_python_tests_LOCATION}/include
    ${OPENCV_MODULE_opencv_stitching_LOCATION}/include
    ${OPENCV_MODULE_opencv_ts_LOCATION}/include
    ${OPENCV_MODULE_opencv_video_LOCATION}/include
    ${OPENCV_MODULE_opencv_videoio_LOCATION}/include
    ${OPENCV_MODULE_opencv_world_LOCATION}/include
    )

#workaround
set(UTILS_DIRECTORY_INCLUDES "../utils")

target_include_directories(${PROJECT_MY_OPENCV} PUBLIC
    ${UTILS_DIRECTORY_INCLUDES}
    ${MY_OPENCV_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR})

target_set_warnings(
    TARGET
    ${PROJECT_MY_OPENCV}
    ENABLE
    ${ENABLE_WARNINGS}
    AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS})

add_linter_tool_to_target(${PROJECT_MY_OPENCV})

if(${ENABLE_LTO})
    target_enable_lto(${PROJECT_MY_OPENCV} optimized)
endif()

add_clang_tidy_msvc_to_target(${PROJECT_MY_OPENCV})
