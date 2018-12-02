add_library(Displaylib display.cpp)
target_include_directories (Displaylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_library(Serverlib )
#target_link_libraries (Serverlibs LINK_PUBLIC Displaylib Threads::Threads m ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline)

add_executable(server server.cpp)
if(APPLE)
    target_link_libraries (server LINK_PUBLIC Displaylib Threads::Threads m ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline)
else()
    target_link_libraries (server LINK_PUBLIC Displaylib Threads::Threads m ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline rt)
endif(APPLE)