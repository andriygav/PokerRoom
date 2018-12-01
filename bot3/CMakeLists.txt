add_library(Bot3lib
        neyron/web.cpp
        neyron/neyron.cpp
        NeyronBot.cpp)
target_include_directories (Bot3lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Bot3 bot_client.cpp)
target_link_libraries (Bot3 LINK_PUBLIC Bot3lib Threads::Threads m)
