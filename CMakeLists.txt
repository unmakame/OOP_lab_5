cmake_minimum_required(VERSION 3.16)
project(OOP_LAB_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем include/
include_directories(${CMAKE_SOURCE_DIR}/include)

# -------- Главная программа --------
add_executable(lab5
    src/main.cpp
    src/allocator.cpp
)

# -------- ТЕСТЫ --------
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(tests
    tests/test_main.cpp
    tests/test_list.cpp
    tests/test_allocator.cpp
    src/allocator.cpp     # реализация аллокатора для тестов
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(tests
    PRIVATE
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
