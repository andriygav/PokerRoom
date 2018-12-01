add_library(Clientlib
        client.cpp
        menu.cpp
        help.cpp
        game.cpp
        button.cpp
        slide.cpp)
target_include_directories (Displaylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(client poker_client.cpp)
target_link_libraries (client LINK_PUBLIC Clientlib Threads::Threads ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES})
