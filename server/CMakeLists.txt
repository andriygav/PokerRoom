add_library(Serverlib display.cpp)
target_link_libraries (Serverlib Threads::Threads m ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline)

add_executable(server server.cpp)
if(APPLE)
    target_link_libraries (server Serverlib)
else()
    target_link_libraries (server Serverlib rt)
endif(APPLE)