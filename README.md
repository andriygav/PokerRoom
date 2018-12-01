# POKER ROOM AND ARTIFFICAL INTELIGENCE BOTS

This project is poker-server with GUI for simple clients and 4 types of bot.

## Installation
### Firstly clone repository
```
git clone https://github.com/andriygav/PokerRoom.git
```
### What libs you nead to have:
```
For Linux Mint17
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
sudo apt-get install libreadline-dev
```

```
For MacOS Mojava
brew install sdl

```

### To compile all bin:

```
make server
make client
make starter
make bot1
make bot2
make bot3
make bot4
```
all bin-files are in folder 'bin', and to "launching all bin you should do only in 'bin'".

## Instraction of program

### Server
#### To start server: 
```
~project/bin $ ./server
```
Server working in local host "127.0.0.1", with port '3425'.
#### Server commands "->":
Get out from server - kill all player.
```
->exit
```
To create room №i = 1..50, if you write i=-1 you create all 50 room.
```
->room -n i
```
To delete room №i = 1..50, if you write i=-1 you delete all 50 room.
```
->delroom -n i
```
To disconnect client №i, if you write i=-1 you disconnect all client.
```
->kill -n i
```
To show log from room №i, if you use [-s] you can see statistic with all player in room, if you use [-f] you can find some log information who start on str.
```
->showroomlog -n i [-s] [-f str]
```
To move to wathing server mod.
```
->display
```
#### Wathing server commands "->display->":
To move to main server mode.
```
->display->exit
```
To watch from room №i = 1..50
```
->display->room -n i
```
All commands you can see, if you double tup 'tab'.
```
->
delroom      display      exit         kill         room         showroomlog
->display->
exit  room

```
Server can complete you commands, if this commands correct.

### Client
#### To start client 
```
~project/bin $ ./client
```
By default it will work in localhost 127.0.0.1, with port '3425'.

Client have two mode - graphic mod and console mode.

By default start graphic mode. 

To open console mode(you can see graphic mode, but you can send some command to server from console).
```
~project/bin $ ./client [-t]
```
#### Three main part:
##### First part - Menu:
When you connect to server - you are in the menu, from menu tou can move to game(in console mode write "game", in graphic mode press button game) and to help(in console mode write "help", in graphic mode press button help), and go out(in console mode write "exit", in graphic mode press button exit)
##### Second part - Help:
You can move back to menu and exit
##### Third part - Game:
In Game, you can move to menu(game not interrupt), disconnect(you leav poker room and lost all your progress).
If Poker Room have 6 people - start game.
In game you have 3 standart poker action - CALL(equate your bet), FOLD(throw off card) and RAISE(raise bet on table on "x" money, in graphic mode slide slider, in console mode write count of money)
For more information press HELP.
Client will create display with user-friendly GUI.

### Starter
With help of this program you can add to game 'n' same bots ( i = 1...4 - number of bots):
```
~project/bin $ ./starter n ./bot[i]_client [-g]
```
### Bots
There are 4 types of bot. bot1 and bot2 are bots with gaming AI (if-bots), and bot3 and bot4 and neyral networks bots.
Each of bots you can launch with comand (where 'i' - is a number of bot i = 1...4)
```
~project/bin $ ./bot'i'_client [-g]
``` 
If you want bot3 to be trained launch it with keys.
```
~project/bin $ ./bot'i'_client [-g] [-f]
``` 

### Testing
We use python for tests: test/testing.py.

## Description of bot
### Bot 1
1. Play in low stack.
2. Try to predict final results and combination of all players.
3. Prediction makes by a lot of random tests.
4. Raise according to his cash.
### Bot 2
1. Play big stack.
2. Count the number of hands that close it at this situation in table.
3. Raise according to his power of combination.
### Bot 3
1. Neyral Network Bot
2. Trying to сopy the actions of players who play with it. 
3. A conventional four-layer perceptron with two hidden layers.
4. Learning is produced by back propogation with stohastic gradient descent.
5. For each round in game using separate network.
### Bot 4
1. Neyron Network Bot
2. Is more independent bot and tries to minimize losses and maximize wins.
3. A conventional six-layer perceptron with four hidden layers.
4. Has embossed edges and has categorial parts.
5. Learning is produced by back propogation with stohastic gradient descent.
6. For each round in game using separate network.



