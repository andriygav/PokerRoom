add_library(Bot3lib
        neyron/web.cpp
        neyron/neyron.cpp
        NeyronBot.cpp)
target_include_directories (Bot3lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bot3_client bot_client.cpp)
target_link_libraries (bot3_client LINK_PUBLIC Bot3lib Threads::Threads m)
