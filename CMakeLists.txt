cmake_minimum_required(VERSION 3.4)
project(Poker)

set(CMAKE_CXX_STANDARD 11)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/log)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL_TTF_INCLUDE_DIRS})

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(bot1)
add_subdirectory(bot2)
add_subdirectory(bot3)
add_subdirectory(bot4)

add_executable(Poker creator/starter.cpp)

#add_custom_target(clean-cmake
#        COMMAND ${CMAKE_COMMAND} -P clean-cmake.cmake
#        )