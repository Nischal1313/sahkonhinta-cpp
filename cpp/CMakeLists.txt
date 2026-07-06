cmake_minimum_required(VERSION 3.16)

project(ElectricityTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(electricity_test
    main.cpp
)

target_link_libraries(electricity_test
    PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)