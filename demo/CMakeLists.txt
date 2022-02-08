add_executable(demo demo.cpp)

target_link_libraries(
    demo
    PRIVATE
    project_warnings
    project_options
    clip
    CONAN_PKG::tau)

install(TARGETS demo DESTINATION ${CMAKE_INSTALL_BINDIR})
