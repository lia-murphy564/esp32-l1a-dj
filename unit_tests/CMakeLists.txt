cmake_minimum_required(VERSION 3.16)
project(unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

enable_testing()
include(GoogleTest)

set(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../main)

# ---------------------------------------------------------------------------
# Fakes shared by every test (Arduino.h, etc.)
# ---------------------------------------------------------------------------
add_library(host_fakes INTERFACE)
target_include_directories(host_fakes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fakes)

# ---------------------------------------------------------------------------
# Code under test: one library per module
# ---------------------------------------------------------------------------

# Header-only
add_library(lmutil INTERFACE)
target_include_directories(lmutil INTERFACE ${MAIN_DIR}/lib)
target_link_libraries(lmutil INTERFACE host_fakes)

# Example of a module with .cpp files (uncomment and adapt):
# add_library(controlbase STATIC
#     ${MAIN_DIR}/lib/ControlBase.cpp)
# target_include_directories(controlbase PUBLIC ${MAIN_DIR}/lib)
# target_link_libraries(controlbase PUBLIC lmutil host_fakes)

# ---------------------------------------------------------------------------
# Test list: each <name> needs unit_tests/test_<name>.cpp and a library target <name>
# ---------------------------------------------------------------------------
set(UNIT_TESTS
    lmutil
)

foreach(name IN LISTS UNIT_TESTS)
    set(src ${CMAKE_CURRENT_SOURCE_DIR}/test_${name}.cpp)
    if(NOT EXISTS ${src})
        message(FATAL_ERROR "Missing test file: unit_tests/test_${name}.cpp")
    endif()
    add_executable(test_${name} ${src})
    target_link_libraries(test_${name} PRIVATE ${name} GTest::gtest_main)
    target_compile_options(test_${name} PRIVATE -Wall -Wextra)
    gtest_discover_tests(test_${name})
endforeach()