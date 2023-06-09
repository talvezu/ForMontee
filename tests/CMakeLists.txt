if(ENABLE_TESTING)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(-Wfatal-errors)
    endif()
    set(CMAKE_CXX_FLAGS  )
    set(TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cc")
    set(TEST_HEADERS "")

    set(GTEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gtest_yaml_api.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/gtest_priority_queue.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/gtest_scheduled_task.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/gtest_database.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/gtest_simple_boost_shared_memory.cpp"
                      "${CMAKE_CURRENT_SOURCE_DIR}/gtest_my_opencv.cpp")
    set(GTEST_HEADERS "")

    add_executable(${UNIT_TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS})
    add_executable(${UNIT_GTEST_NAME} ${GTEST_SOURCES} ${GTEST_HEADERS})

    target_link_libraries(${UNIT_TEST_NAME} PUBLIC ${LIBRARY_NAME})
    target_link_libraries(${UNIT_TEST_NAME} PRIVATE Catch2::Catch2)

    #workaround
    set(UTILS_DIRECTORY_INCLUDES "../app/utils")
    set(OPENCV_DIRECTORY_INCLUDES "../app/opencv")
    target_include_directories(${UNIT_GTEST_NAME} PUBLIC ${GTEST_INCLUDE_DIR} ${YAML_LIBRARY_INCLUDES} ${UTILS_DIRECTORY_INCLUDES} ${OPENCV_DIRECTORY_INCLUDES})
    target_link_libraries(${UNIT_GTEST_NAME} PUBLIC ${LIBRARY_NAME} yaml-cpp re2 ${PROJECT_UTILS} ${YAML_LIBRARY_NAME} ${PROJECT_MY_OPENCV})
    target_link_libraries(${UNIT_GTEST_NAME} PRIVATE  GTest::gtest_main)

    #gtest_discover_tests(${UNIT_GTEST_NAME})

    target_set_warnings(
        TARGET
        ${UNIT_TEST_NAME}
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})

    target_set_warnings(
        TARGET
        ${UNIT_GTEST_NAME}
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})

    add_linter_tool_to_target(${UNIT_TEST_NAME})
    add_linter_tool_to_target(${UNIT_GTEST_NAME})

    # skip for now for gtest
    if(ENABLE_COVERAGE)
        set(COVERAGE_MAIN "coverage")
        set(COVERAGE_EXCLUDES
            "${PROJECT_SOURCE_DIR}/app/*"
            "${PROJECT_SOURCE_DIR}/cmake/*"
            "${PROJECT_SOURCE_DIR}/docs/*"
            "${PROJECT_SOURCE_DIR}/external/*"
            "${PROJECT_SOURCE_DIR}/tests/*"
            "${PROJECT_SOURCE_DIR}/build/*"
            "/usr/include/*")
        setup_target_for_coverage_lcov(
            NAME
            ${COVERAGE_MAIN}
            EXECUTABLE
            ${UNIT_TEST_NAME}
            DEPENDENCIES
            ${UNIT_TEST_NAME})
    endif()
endif()
