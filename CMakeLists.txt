cmake_minimum_required(VERSION 3.15)

project(clip CXX)

include(${CMAKE_CURRENT_LIST_DIR}/cmake_includes/setup_project.cmake)
setup_project()

add_subdirectory(clip)

if (ENABLE_TESTING)
    if (${PROJECT_IS_TOP_LEVEL} OR ${RECURSIVE_BUILD_TESTS})
        add_subdirectory(test)
    endif ()
endif ()

if (${PROJECT_IS_TOP_LEVEL})
    option(
        BUILD_DEMO
        "Build the demo"
        ${ENABLES_DEFAULT})

    if (${BUILD_DEMO})
        add_subdirectory(demo)
    endif ()

    option(
        BUILD_TOOLS
        "Build the tools"
        ${ENABLES_DEFAULT})

    if (${BUILD_TOOLS})
        add_subdirectory(tools)
    endif ()
endif ()
