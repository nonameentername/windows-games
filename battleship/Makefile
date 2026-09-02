CXX=g++
CC=gcc

SOURCE=final.cpp
PROJECT_ROOT=..
SDL_BGI_DIR=$(PROJECT_ROOT)/SDL2_bgi-3.0.4/src
SDL_BGI_LIB=$(SDL_BGI_DIR)/libSDL2_bgi.a

LIBS= \
	$(SDL_BGI_LIB) \
	$(shell pkg-config --libs sdl2) \
	-lm \

INCLUDE=-I$(PROJECT_ROOT)/compat -I$(SDL_BGI_DIR) $(shell pkg-config --cflags sdl2)

OUTPUT=battle_spaceship.exe

CPPFLAGS=-Wall -Wno-write-strings

all: $(SDL_BGI_LIB)
	$(CXX) -g -o $(OUTPUT) $(CPPFLAGS) $(SOURCE) $(LIBS) $(INCLUDE)

$(SDL_BGI_LIB): $(SDL_BGI_DIR)/SDL2_bgi.c $(SDL_BGI_DIR)/SDL2_bgi.h $(SDL_BGI_DIR)/graphics.h
	$(CC) -g -O2 -std=gnu99 -Wall $(shell pkg-config --cflags sdl2) -c $(SDL_BGI_DIR)/SDL2_bgi.c -o $(SDL_BGI_DIR)/SDL2_bgi.o
	ar rcs $(SDL_BGI_LIB) $(SDL_BGI_DIR)/SDL2_bgi.o

clean:
	rm -rf $(OUTPUT)
