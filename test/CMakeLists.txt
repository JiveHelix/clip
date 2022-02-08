# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(clip_catch_main STATIC catch_main.cpp)
target_link_libraries(clip_catch_main PUBLIC CONAN_PKG::catch2)
target_link_libraries(clip_catch_main PRIVATE project_options)

add_executable(
    clip_tests
    dictionary_tests.cpp
    channel_layout_tests.cpp
    sample_format_tests.cpp)

target_link_libraries(
    clip_tests
    PRIVATE
    project_warnings
    project_options
    clip_catch_main
    clip)

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. TEST_PREFIX to whatever you want, or use different
# for different binaries
catch_discover_tests(
    clip_tests
    TEST_PREFIX
    "unittests."
    EXTRA_ARGS)
