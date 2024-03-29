cmake_minimum_required(VERSION 3.16)

project(
    "ConfigurableTaskManager"
    VERSION 1.0.0
    LANGUAGES C CXX)

# Global CMake variables are set here
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Options
option(USE_CONAN "Whether to use conan" OFF)
option(USE_VCPKG "Whether to use vcpkg" OFF)

option(ENABLE_WARNINGS "Enable to add warnings to a target." ON)
option(ENABLE_WARNINGS_AS_ERRORS "Enable to treat warnings as errors." OFF)

option(ENABLE_TESTING "Enable a Unit Testing build." ON)
option(ENABLE_COVERAGE "Enable a Code Coverage build." OFF)

option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable to add iwyu." OFF)
option(ENABLE_CLANG_TIDY "Enable to add clang tidy." ON)
option(ENABLE_CPPCHECK "Enable to add cppcheck." OFF)

option(ENABLE_SANITIZE_ADDR "Enable to sanitize address (Unix only)." OFF)
option(ENABLE_SANITIZE_UNDEF "Enable to sanitize undefined (Unix only)." OFF)
option(ENABLE_SANITIZE_LEAK "Enable to sanitize undefined (Unix only)." OFF)

option(ENABLE_CLANG_FORMAT "Enable to add clang-format." ON)
option(ENABLE_CMAKE_FORMAT "Enable to add cmake-format." ON)

option(ENABLE_LTO "Enable to add Link Time Optimization." ON)

# Project/Library Names
set(LIBRARY_NAME "lib")
set(YAML_LIBRARY_NAME "yaml_lib")
set(PROJECT_UTILS "project_util")
set(UNIT_TEST_NAME "unit_tests")
set(UNIT_GTEST_NAME "unit_gtests")
set(EXECUTABLE_NAME "main")
set(PROJECT_MY_OPENCV "my_opencv")

# CMAKE MODULES

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
include(FetchContent)

include(AddGitSubmodule)
include(Docs)
include(Warnings)
include(Tools)
include(LTO)
include(ConfigSafeGuards)
include(DetectSystem)

detect_system()
if (${SYSTEM_WSL})
    message("running on WSL")
endif()

add_git_submodule(external/opencv)

add_cmake_format_target()
add_clang_format_target()

if(ENABLE_SANITIZE_ADDR OR ENABLE_SANITIZE_UNDEF)
    include(Sanitizer)
    add_sanitizer_flags()
endif()

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

if(ENABLE_LTO)
    find_lto(CXX)
endif()

# EXTERNAL LIBRARIES

if(USE_CONAN)
    message("==> Using Conan")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    find_package(re2)
    find_package(yaml-cpp)
    find_package(nlohmann_json)
    find_package(fmt)
    find_package(spdlog)
    find_package(Catch2)
    find_package(cxxopts)
elseif(USE_VCPKG)
    message("==> Using VCPKG")
    include(${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake)
    find_package(re2)
    find_package(yaml-cpp)
    find_package(nlohmann_json)
    find_package(fmt)
    find_package(spdlog)
    find_package(Catch2)
    find_package(cxxopts)
else() # Use FetchContent
    message("==> Using FetchContent")

    FetchContent_Declare(
        re2
        GIT_REPOSITORY https://github.com/google/re2.git
        GIT_TAG 2023-03-01
      )

    FetchContent_MakeAvailable(re2)

    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.6.3
      )

    FetchContent_MakeAvailable(yaml-cpp)

    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(nlohmann_json)

    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(fmt)

    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(spdlog)

    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG v3.0.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(cxxopts)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.9
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(googletest)
endif()

# SUB DIRECTORIES

add_subdirectory(configured)
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)

# INSTALL TARGETS

install(
    TARGETS ${EXECUTABLE_NAME}
    EXPORT ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(
    TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
