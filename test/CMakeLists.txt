include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)

# Отключаем установку для Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}.test
  AllTests.cpp
  tests.cpp
)

target_link_libraries(${PROJECT_NAME}.test
    ${PROJECT_NAME}
    gtest_main
    gmock_main
)

if(MSVC)
  target_compile_options(${PROJECT_NAME}.test PRIVATE /bigobj)
endif()

install(TARGETS ${PROJECT_NAME}.test
        RUNTIME DESTINATION "${tool_dest}")