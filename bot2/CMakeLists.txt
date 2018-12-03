add_executable(bot2_client bot_client.cpp client.cpp client_menu.h client_help.h client_game.h)
target_link_libraries (bot2_client Threads::Threads m)