# Покерная комната

Этот проект посвящен написанию удаленного сервера и клиента для игры в покер. В качестве противника также могут быть автоматические боты.

## Installation
### Сперва нужно загрузить этот репозиторий
```
git clone https://github.com/andriygav/PokerRoom.git
```
### Для корректной работы вам нужны следующие библиотеки
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

### Компиляция всего проекта:

```
~PokerRoom $ cmake ./
~PokerRoom $ make
```
Все исполняемые файлы находятся в папке bin и запуск нужно осуществлять из неё.

## Инструкция по работе с проектом

### Server
#### Запуск сервера
По умолчанию сервер работает на локальном адресе "127.0.0.1" с портом "3425". Для того чтобы запустить на другом введите через аргумент командой строки.
```
~PokerRoom/bin $ ./server [-p port] [-a address]
```
#### Команды сервера "->":
Вы можете увидеть все команды нажав дважды 'tab'. Также работает автодополнение.
```
->
delroom      display      exit         kill         room         showroomlog
->display->
exit  room

```
Отключить сервер:
```
->exit
```
Для создания комнат №i = 1..50, или же создание всех сразу i=-1 команда ниже.
```
->room -n i
```
Для удаления комнаты №i = 1..50, или же удаления всех сразу i=-1 команда ниже.
```
->delroom -n i
```
Для отключениия клиента №i(отключить всех сразу i=-1) от сервера команда ниже.
```
->kill -n i
```
Для вывода логов комнаты №i используется команда ниже. Где введены флаги [-s] это вывод статистики по всем игрокам в комнате, и флаг [-f] для поиска по логам, найдет тот лог который начинается со строки str.
```
->showroomlog -n i [-s] [-f str]
```
Также доступен режим наблюдения при помощи sdl:
```
->display
```
#### Команды режима "->display->":
Чтобы вернуться в основной режим сервера нужна команда ниже.
```
->display->exit
```
Для просмотра комнаты №i = 1..50 используется следующая команда:
```
->display->room -n i
```

### Client
#### Для запуска клиента
По умолчанию клиент запускается на адресе "127.0.0.1" с портом "3425". Для запуска под другими воспользуйтесь аргументами командной строки. Для запуска только консольной версиии(без sdl) воспользуйтесь аргументом [-T]. [-t] запуск смешаного режима.
```
~PokerRoom/bin $ ./client [-T] [-t] [-p port] [-a address]
```

Клиент имеет два режима --- графический и консольный режим.

По умолчанию запускается графический режим.

#### Команды клиента "->":
##### menu:
```
->menu->
```
После подключеня к серверу вы перенаправляетесь в меню. Из меню вы можете перейти в саму игру(введя game в консоли или же нажав на кнопку game в графическом интерфейсе), также можете перейти в помощь(введя help в консоли или же нажав на кнопку help в графическом интерфейсе). Чтобы выйти из клииента введите или нажмите "exit".
Также присутствуют подсказки при двойном нажатии клавиши "tab".
##### help:
```
->help->
```
Из вкладки помощь вы можете перейти в саму меню(введя menu в консоли или же нажав на кнопку menu в графическом интерфейсе). Чтобы выйти из клиента введите или нажмите "exit".
Также присутствуют подсказки при двойном нажатии клавиши "tab".
##### game:
```
->game->
```
Из игры вы можете перейти в меню или помощь при нажатии или вводе соответсвующей команды. Также есть команда disconnect(после ввода вы покидаете комнату и теряете все текущие достижение).
Игра начинается только после того, как в комнату зайдет 6 человек и начнут игру.
В игре присутствует 3 стандратных действия:
CALL(принять ставку)
```
->game->call
```
FOLD(сбрость карты)
```
->game->fold
```
RAISE(поднять ставку на "x" денег, в графическом моде используйте ползунок, в консольном моде введите число)
```
->game->raise x
```
Для большей информации перейдите в help.

### Starter
Для простоты, можно добавить в игру сразу несколько ботов для этого используется команда ниже, где [-g] флаг нужен, чтобы боты сразу перешли в игру с меню. n --- это количество ботов. i = 1...4 --- это номер бота.
```
~PokerRoom/bin $ ./Poker n ./bot[i]_client [-g] [-p port] [-a address]
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



