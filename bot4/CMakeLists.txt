add_library(Bot4lib
        neyron/web.cpp
        neyron/neyron.cpp
        NeyronBot.cpp)
target_include_directories (Bot4lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bot4_client bot_client.cpp)
target_link_libraries (bot4_client LINK_PUBLIC Bot4lib Threads::Threads m)