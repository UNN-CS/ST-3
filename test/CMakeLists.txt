find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}.test
  AllTests.cpp
  tests.cpp)

target_link_libraries(${PROJECT_NAME}.test
    ${PROJECT_NAME}
    GTest::gtest
    GTest::gmock
    Threads::Threads
)

add_test(NAME ${PROJECT_NAME}.test COMMAND ${PROJECT_NAME}.test)

install(TARGETS ${PROJECT_NAME}.test
        DESTINATION "${tool_dest}")
