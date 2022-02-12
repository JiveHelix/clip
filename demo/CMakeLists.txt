add_executable(audio_and_video audio_and_video.cpp)

target_link_libraries(
    audio_and_video
    PRIVATE
    project_warnings
    project_options
    clip)


add_executable(video video.cpp)

target_link_libraries(
    video
    PRIVATE
    project_warnings
    project_options
    clip)


add_executable(lossless_video lossless_video.cpp)

target_link_libraries(
    lossless_video
    PRIVATE
    project_warnings
    project_options
    clip)


install(
    TARGETS
    audio_and_video
    video
    lossless_video
    DESTINATION
    ${CMAKE_INSTALL_BINDIR})
