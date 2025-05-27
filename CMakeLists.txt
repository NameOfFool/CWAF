cmake_minimum_required(VERSION 3.19)
project(CWAF LANGUAGES CXX)
set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)
find_package(Boost REQUIRED)

add_executable(CWAF "src/main.cpp")

target_link_libraries(CWAF PRIVATE Boost::boost)
