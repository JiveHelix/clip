add_catch2_test(
    NAME clip_tests
    SOURCES
        channel_layout_tests.cpp
        dictionary_tests.cpp
        sample_format_tests.cpp
    LINK
        clip)
