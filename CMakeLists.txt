cmake_minimum_required(VERSION 3.15)

project(clip CXX)

include(${CMAKE_CURRENT_LIST_DIR}/cmake_includes/setup_project.cmake)
setup_project()

add_subdirectory(clip)

include(${CMAKE_CURRENT_LIST_DIR}/cmake_includes/enable_extras.cmake)
enable_extras()

if (PROJECT_IS_TOP_LEVEL)
    if (EXISTS "tools/CMakeLists.txt")
        add_subdirectory(tools)
    endif ()

    if (EXISTS "demo/CMakeLists.txt")
        add_subdirectory(demo)
    endif ()
endif ()
