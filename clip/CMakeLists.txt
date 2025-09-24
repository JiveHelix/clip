add_library(clip INTERFACE)

find_package(Fmt REQUIRED)
find_package(Ffmpeg REQUIRED)
find_package(Tau REQUIRED)

# Projects that include this project use #include "clip/<header-name>"
target_include_directories(clip INTERFACE ${PROJECT_SOURCE_DIR})

target_link_libraries(
    clip
    INTERFACE
    fmt::fmt
    tau::tau
    ffmpeg::ffmpeg)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/clip
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
