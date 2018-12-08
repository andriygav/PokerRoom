# PokerRoom

This project is dedicated to make a remote server and client for Poker game. Automatic bot can also be as an opponents.


## Installation

### Firstly, download repository
```
git clone https://github.com/andriygav/PokerRoom.git
```
You need the followiong libraries for correct work
### You need the followiong libraries for correct work
##### Linux (Ubuntu)
```
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
sudo apt-get install libreadline-dev
sudo apt-get install cmake
```

##### MacOS Mojava
```
brew install sdl
brew install sdl_ttf
brew install cmake
```

### Compilation of the whole project:

```
~PokerRoom $ mkdir build && cd build
~PokerRoom/build $ cmake ../
~PokerRoom/build $ make
```
All executable files are in bin folder and the launch must be carried out from it.

## Instructions for working with project

### Server
#### Server startup
By defult server works on the local address "127.0.0.1" with port "3425". To lounch it on another address use the keys "-p" and "-a" for new port and address respectively. "-d" option is being used to run the server in the background.
```
~PokerRoom/bin $ ./server [-p port] [-a address] [-d]
```
#### Server commands "->":
You can see all the existing commands using double 'tab'. Auto-completion also works.
```
->
delroom      display      exit         kill         room         showroomlog
->display->
exit  room

```
Turn off server:
```
->exit
```
To create poker rooms №i = 1..50.
```
->room -n i
```
To remove a room №i = 1..50.
```
->delroom -n i
```
To disconnect a client №i from server.
```
->kill -n i
```
To output the logs of the room №i use the command below. Use [-s] flag to output the all players statistics and [-f] flag to find by log. Can only find log begining from the string str.

```
->showroomlog -n i [-s] [-f str]
```
It is also monitoring mode avaliable using stl:
```
->display
```
#### Mode command "->display->":
To return to the main server mode, you need the command below.
```
->display->exit
```
To observe the room №i = 1..50 use the following command:
```
->display->room -n i
```

### Client
#### To start the client
By default client launches on address "127.0.0.1" with port "3425". To launch on another address use arguments [-p port] and [-a address] for changing the port and address respectively. To launch only a console version(without sdl) use an argument [-T]. Argument [-t] is for mixed mode(console + sdl).

```
~PokerRoom/bin $ ./client [-T] [-t] [-p port] [-a address]
```

Client has two modes: graphic and console.

By default grafic mode starts. 

#### Client commands "->":
##### menu:
```
->menu->
```
After connecting to server you will be redirrected to menu. From menu you can start the game (typing "game" in the console or by clicking on the "game" button in the graphical interface), also you can go to help (typing "help" in the console or by clicking on the "help" button in the graphical interface). To exit the client press "exit". Also, you can see all the existing commands using double 'tab'.

##### help:
```
->help->
```

From help you may go to menu (typing "menu" in the console or by clicking on the "menu" button in the graphical interface).
To exit from client type "exit". Also, you can see all the existing commands using double 'tab'.
##### game:
```
->game->
```
From game you may go to menu or help by clicking appropriate command. Also, there is "disconnect" command which allows you to leave the room, but you lose lose all current achievements.
The game starts when 6 players start game.
There 
Из игры вы можете перейти в меню или помощь при нажатии или вводе соответсвующей команды. Также есть команда disconnect(после ввода вы покидаете комнату и теряете все текущие достижение).
Игра начинается только после того, как в комнату зайдет 6 человек и начнут игру.
The game has 3 standard actions:

CALL (to accept bet)
```
->game->call
```
FOLD (to discard cards)
```
->game->fold
```
RAISE (to raise bet for x currency units(use scroller in grafic inteface version))
```
->game->raise x
```
For more detail information go to help.

### Starter
For simplicity, you can invite bots to game by using the command below, where [-g] flag is for redirrecting bots to game immediately after creating them. "n" is the number of bots you want to create and "i" (i = 1...4) is the identificator of bot.
```
~PokerRoom/bin $ ./Poker n ./bot[i]_client [-g] [-p port] [-a address]
```
### Bots
There are 4 types of bot. bot1 and bot2 are bots with gaming AI (if-bots), and bot3 and bot4 and neyral networks bots.
Each of bots you can launch with command (where 'i' - is a number of bot i = 1...4)
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



