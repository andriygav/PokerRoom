add_library(Bot4lib
        neyron/web.cpp
        neyron/neyron.cpp
        NeyronBot.cpp)
target_include_directories (Bot4lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Bot4 bot_client.cpp)
target_link_libraries (Bot4 LINK_PUBLIC Bot4lib Threads::Threads m)