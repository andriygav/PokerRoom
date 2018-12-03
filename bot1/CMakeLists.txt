add_executable(bot1_client
        bot_client.cpp
        client.cpp
        client_help.h
        client_menu.h
        client_game.h)
target_link_libraries (bot1_client Threads::Threads m)