set(UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ThreadUtil.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PeriodicTask.cpp")

set(UTILS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/ThreadUtil.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/logging.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/Storage.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_queue.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PriorityQueueElement.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PriorityQ.h"
                  "${CMAKE_CURRENT_SOURCE_DIR}/PeriodicTask.h")


set(UTILS_INCLUDES "./")

add_library(${PROJECT_UTILS} STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_UTILS} PUBLIC Threads::Threads)

target_include_directories(${PROJECT_UTILS} PUBLIC ${UTILS_INCLUDES})

target_link_libraries(
    ${PROJECT_UTILS}
    PUBLIC Threads::Threads)

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





