add_library(Displaylib display.cpp)
target_include_directories (Displaylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(server server.cpp)
target_link_libraries (server LINK_PUBLIC Displaylib Threads::Threads m rt ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline)
