add_library(clip INTERFACE)

# Projects that include this project use #include "clip/<header-name>"
target_include_directories(clip INTERFACE ${PROJECT_SOURCE_DIR})

target_link_libraries(
    clip
    INTERFACE
    CONAN_PKG::fmt
    CONAN_PKG::jive
    CONAN_PKG::tau
    CONAN_PKG::ffmpeg)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/clip
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
