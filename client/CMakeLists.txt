add_library(Clientlib
        client.cpp
	admin.cpp
        menu.cpp
        help.cpp
        game.cpp
        button.cpp
        slide.cpp)

add_executable(client poker_client.cpp)
target_link_libraries (client Clientlib Threads::Threads ${SDL_LIBRARY} ${SDL_TTF_LIBRARIES} readline)
