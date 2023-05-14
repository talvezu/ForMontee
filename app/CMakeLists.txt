set(APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cc" "${CMAKE_CURRENT_SOURCE_DIR}/TasksFromConfig.cpp")
set(APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/TasksFromConfig.h" "${CMAKE_CURRENT_SOURCE_DIR}/InteractiveTask.h")

set(UTILS_DIRECTORY_INCLUDES "./"
                             "${CMAKE_BINARY_DIR}/configured_files/include"
                             "${CMAKE_CURRENT_SOURCE_DIR}/utils")

add_subdirectory(utils)

add_executable(${EXECUTABLE_NAME} ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_UTILS})



target_link_libraries(
    ${EXECUTABLE_NAME}
    PUBLIC ${LIBRARY_NAME}
           ${YAML_LIBRARY_NAME}
           ${PROJECT_UTILS}
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
