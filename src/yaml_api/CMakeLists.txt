# Sources and Headers
set(YAML_LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/yaml_api.cpp")
set(YAML_LIBRARY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/yaml_api.h")
set(YAML_LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

# MyLib Library
add_library(${YAML_LIBRARY_NAME} STATIC ${YAML_LIBRARY_SOURCES} ${YAML_LIBRARY_HEADERS})
target_include_directories(${YAML_LIBRARY_NAME} PUBLIC ${YAML_LIBRARY_INCLUDES})
target_link_libraries(
    ${YAML_LIBRARY_NAME}
    PRIVATE yaml-cpp
            re2
            nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts)

target_set_warnings(
    TARGET
    ${YAML_LIBRARY_NAME}
    ENABLE
    ${ENABLE_WARNINGS}
    AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS})

add_linter_tool_to_target(${YAML_LIBRARY_NAME})

if(${ENABLE_LTO})
    target_enable_lto(${YAML_LIBRARY_NAME} optimized)
endif()

add_clang_tidy_msvc_to_target(${YAML_LIBRARY_NAME})
