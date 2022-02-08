add_executable(list_encoders list_encoders.cpp)

target_link_libraries(
    list_encoders
    PRIVATE
    clip)

install(TARGETS list_encoders DESTINATION ${CMAKE_INSTALL_BINDIR})
