.PHONY: bot4 bot3 bot2 bot1 client server starter

bot1:
	mkdir bin -p
	mkdir bin/log -p
	g++ bot1/bot_client.cpp -o bin/bot1_client -pthread
bot2:
	mkdir bin -p
	mkdir bin/log -p
	g++ bot2/bot_client.cpp -o bin/bot2_client -pthread

bot3:
	mkdir bin -p
	mkdir bin/log -p
	mkdir bin/Bot3 -p
	g++ -c bot3/neyron/web.cpp -o bot3/neyron/web.o
	ar -r bot3/neyron/libweb.a bot3/neyron/web.o
	g++ -c bot3/neyron/neyron.cpp -o bot3/neyron/neyron.o
	ar -r bot3/neyron/libneyron.a bot3/neyron/neyron.o
	g++ -c bot3/NeyronBot.cpp -o bot3/NeyBot.o
	ar -r bot3/libneybot.a bot3/NeyBot.o
	g++ bot3/bot_client.cpp -o bin/bot3_client -pthread bot3/libneybot.a bot3/neyron/libweb.a bot3/neyron/libneyron.a  -lm

bot4:
	mkdir bin -p
	mkdir bin/log -p
	mkdir bin/Bot4 -p
	g++ -c bot4/neyron/web.cpp -o bot4/neyron/web.o
	ar -r bot4/neyron/libweb.a bot4/neyron/web.o
	g++ -c bot4/neyron/neyron.cpp -o bot4/neyron/neyron.o
	ar -r bot4/neyron/libneyron.a bot4/neyron/neyron.o
	g++ -c bot4/NeyronBot.cpp -o bot4/NeyBot.o
	ar -r bot4/libneybot.a bot4/NeyBot.o
	g++ bot4/bot_client.cpp -o bin/bot4_client -pthread bot4/libneybot.a bot4/neyron/libweb.a bot4/neyron/libneyron.a  -lm

server: 
	mkdir bin -p
	mkdir bin/log -p
	g++ -c server/display.cpp -o server/display.o
	ar -r server/libdisplay.a server/display.o
	g++ server/server.cpp -o bin/server -lrt -pthread server/libdisplay.a -lSDL -lSDL_ttf -lpthread -lreadline

starter:
	mkdir bin -p
	g++ creator/starter.cpp -o bin/starter

client: 
	mkdir bin -p
	mkdir bin/log -p
	g++ -c client/client.cpp -o client/client.o
	ar -r client/libclient.a client/client.o
	g++ -c client/menu.cpp -o client/menu.o
	ar -r client/libmenu.a client/menu.o
	g++ -c client/help.cpp -o client/help.o
	ar -r client/libhelp.a client/help.o
	g++ -c client/game.cpp -o client/game.o
	ar -r client/libgame.a client/game.o
	g++ -c client/button.cpp -o client/button.o
	ar -r client/libbutton.a client/button.o
	g++ -c client/slide.cpp -o client/slide.o
	ar -r client/libslide.a client/slide.o
	g++ client/poker_client.cpp -o bin/client client/libclient.a client/libmenu.a client/libhelp.a client/libgame.a client/libslide.a client/libbutton.a -lSDL -lSDL_ttf -lpthread

