# windows-games

These games were originally written for Windows but have since been ported over to SDL2

![Celda](./images/celda-1.png)
![Celda](./images/celda-2.png)
![Celda](./images/celda-3.png)
![Battleship](./images/battleship-1.png)
![Battleship](./images/battleship-2.png)
![Tetris](./images/tetris-1.png)
![Tetris](./images/tetris-2.png)

This project uses SDL_bgi: https://sourceforge.net/projects/sdl-bgi/

Requires the following dependencies:

    sudo apt-get install -y make build-essential pkg-config libsdl2-dev

To build the games:

    make

Optionally build using docker:

    docker compose build
    docker compose run --rm make

The compiled games can then be run in linux:

    battleship/battle_spaceship.exe
    celda/celda.exe
    tetris/tetris.exe
