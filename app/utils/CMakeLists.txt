set(UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ThreadUtil.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PeriodicTask.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/DB.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/SimpleBoostSharedMemory.cpp")

set(UTILS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/ThreadUtil.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/DB.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/global_settings.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/Storage.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_queue.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PriorityQueueElement.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PriorityQ.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PeriodicTask.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/SimpleBoostSharedMemory.h")


set(UTILS_INCLUDES "./")

include(FetchContent)
if(USE_CONAN)
    message("==> Using Conan")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    find_package(pybind11)
elseif(USE_VCPKG)
    message("==> Using VCPKG")
    include(${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake)
    find_package(pybind11)
else() # Use FetchContent
    message("==> app/utils Using FetchContent")

    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG v2.7.1
      )

    FetchContent_MakeAvailable(pybind11)
    message("==> app/utils pybind11 source dir ${pybind11_SOURCE_DIR}")
    message("==> app/utils pybind11 bin dir ${pybind11_BINARY_DIR}")
    message("==> app/utils pybind11 include dir ${pybind11_INCLUDE_DIRS}")
    # Find the Python interpreter and its development headers
endif()


add_library(${PROJECT_UTILS} STATIC ${UTILS_SOURCES} ${UTILS_HEADERS} ${pybind11_INCLUDE_DIRS})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_UTILS} PUBLIC Threads::Threads)

target_include_directories(${PROJECT_UTILS} PUBLIC ${UTILS_INCLUDES})

target_link_libraries(
    ${PROJECT_UTILS}
    PUBLIC pybind11::embed Threads::Threads)

target_set_warnings(
    TARGET
    ${PROJECT_UTILS}
    ENABLE
    ${ENABLE_WARNINGS}
    AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS})

add_linter_tool_to_target(${PROJECT_UTILS})

if(${ENABLE_LTO})
    target_enable_lto(${PROJECT_UTILS} optimized)
endif()

add_clang_tidy_msvc_to_target(${PROJECT_UTILS})

pybind11_add_module(python_api_shared_data "${CMAKE_CURRENT_SOURCE_DIR}/DB.cpp")



