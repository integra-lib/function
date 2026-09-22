cmake_minimum_required(VERSION 3.21)

project(
  integra_function
  VERSION 0.1.0
  LANGUAGES CXX)

add_library(integra_function INTERFACE)
add_library(Integra::function ALIAS integra_function)

target_include_directories(integra_function
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(integra_function INTERFACE cxx_std_20)

# Dependants read the version from this property rather than from a variable: a
# variable set here is invisible in a sibling directory, and a stale one left in
# the parent scope would be read instead — a version check that silently passes.
set_target_properties(integra_function PROPERTIES INTEGRA_VERSION
                                                  ${PROJECT_VERSION})

if(PROJECT_IS_TOP_LEVEL)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2)
  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
  enable_testing()

  add_executable(function-test tests/function_test.cpp)
  target_link_libraries(function-test PRIVATE Integra::function
                                              GTest::gtest_main)
  gtest_discover_tests(function-test)
endif()
