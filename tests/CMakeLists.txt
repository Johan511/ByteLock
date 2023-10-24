add_executable(tests ${CMAKE_CURRENT_SOURCE_DIR}/unit/lock_correctness_test.cpp)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_link_libraries(tests range_lock)