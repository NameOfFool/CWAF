cmake_minimum_required(VERSION 3.21)
project(CWAF LANGUAGES CXX)
set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

find_package(Poco REQUIRED Util Net)


#libre2
include_directories("/usr/include/re2/re2.h")
link_directories("/usr/lib/libre2.so")

file(GLOB SOURCES
   "src/*"
)

add_executable(CWAF ${SOURCES})

target_include_directories(CWAF PRIVATE "include")
target_link_libraries(CWAF PRIVATE Poco::Util Poco::Net re2)
