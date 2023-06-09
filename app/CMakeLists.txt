set(APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cc"
                "${CMAKE_CURRENT_SOURCE_DIR}/TasksFromConfig.cpp")
set(APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/TasksFromConfig.h"
                "${CMAKE_CURRENT_SOURCE_DIR}/InteractiveTask.h"
                "${CMAKE_CURRENT_SOURCE_DIR}/NetFunctionFactory.h")

set(UTILS_DIRECTORY_INCLUDES "./"
                             "${CMAKE_BINARY_DIR}/configured_files/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/utils")

set(OPENCV_DIRECTORY_INCLUDES "./"
                             "${CMAKE_CURRENT_SOURCE_DIR}/opencv")


add_subdirectory(utils)
add_subdirectory(opencv)

add_executable(${EXECUTABLE_NAME} ${APP_SOURCES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_UTILS} ${OPENCV_DIRECTORY_INCLUDES})

target_link_libraries(
    ${EXECUTABLE_NAME}
    PUBLIC ${LIBRARY_NAME}
           ${YAML_LIBRARY_NAME}
           ${PROJECT_UTILS}
           ${PROJECT_MY_OPENCV}
           nlohmann_json::nlohmann_json
           re2
           yaml-cpp
           fmt::fmt
           spdlog::spdlog
           cxxopts::cxxopts)

target_set_warnings(
    TARGET
    ${EXECUTABLE_NAME}
    ENABLE
    ${ENABLE_WARNINGS}
    AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS})
add_linter_tool_to_target(${EXECUTABLE_NAME})

if(${ENABLE_LTO})
    target_enable_lto(${EXECUTABLE_NAME} optimized)
endif()

add_clang_tidy_msvc_to_target(${EXECUTABLE_NAME})
