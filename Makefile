.PHONY: all web web-battleship web-celda web-tetris deb-dep clean clean-web

WEB_BUILD_DIR=build/web
SDL_BGI_DIR=SDL2_bgi-3.0.4/src
WEB_SDL_BGI_LIB=$(WEB_BUILD_DIR)/libSDL2_bgi.a
WEB_SHELL=web/shell.html
WEB_CFLAGS=-O2 -std=gnu99 -Wall -sUSE_SDL=2 -sASYNCIFY=1 -Icompat -I$(SDL_BGI_DIR)
WEB_CXXFLAGS=-O2 -Wall -Wno-write-strings -sUSE_SDL=2 -sASYNCIFY=1 -sALLOW_MEMORY_GROWTH=1 -sMINIFY_HTML=0 -Icompat -I$(SDL_BGI_DIR)

all:
	$(MAKE) -C battleship
	$(MAKE) -C celda
	$(MAKE) -C tetris

web: web-battleship web-celda web-tetris

$(WEB_SDL_BGI_LIB): $(SDL_BGI_DIR)/SDL2_bgi.c $(SDL_BGI_DIR)/SDL2_bgi.h $(SDL_BGI_DIR)/graphics.h
	mkdir -p $(WEB_BUILD_DIR)
	emcc $(WEB_CFLAGS) -c $(SDL_BGI_DIR)/SDL2_bgi.c -o $(WEB_BUILD_DIR)/SDL2_bgi.o
	emar rcs $(WEB_SDL_BGI_LIB) $(WEB_BUILD_DIR)/SDL2_bgi.o

web-battleship: $(WEB_SDL_BGI_LIB) $(WEB_SHELL)
	mkdir -p $(WEB_BUILD_DIR)/battle_spaceship
	em++ $(WEB_CXXFLAGS) --shell-file $(WEB_SHELL) --preload-file battleship battleship/final.cpp $(WEB_SDL_BGI_LIB) -o $(WEB_BUILD_DIR)/battle_spaceship/index.html

web-celda: $(WEB_SDL_BGI_LIB) $(WEB_SHELL)
	mkdir -p $(WEB_BUILD_DIR)/celda
	em++ $(WEB_CXXFLAGS) --shell-file $(WEB_SHELL) --preload-file celda celda/final.cpp $(WEB_SDL_BGI_LIB) -o $(WEB_BUILD_DIR)/celda/index.html

web-tetris: $(WEB_SDL_BGI_LIB) $(WEB_SHELL)
	mkdir -p $(WEB_BUILD_DIR)/tetris
	em++ $(WEB_CXXFLAGS) --shell-file $(WEB_SHELL) tetris/tetris.cpp $(WEB_SDL_BGI_LIB) -o $(WEB_BUILD_DIR)/tetris/index.html

deb-dep:
	sudo apt-get install -y make build-essential pkg-config libsdl2-dev

clean: $(SUBDIRS)
	$(MAKE) -C battleship $@
	$(MAKE) -C celda $@
	$(MAKE) -C tetris $@

clean-web:
	rm -rf $(WEB_BUILD_DIR)

publish:
	cp -r $(WEB_BUILD_DIR)/* public
