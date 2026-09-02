all:
	$(MAKE) -C battleship
	$(MAKE) -C celda
	$(MAKE) -C tetris

deb-dep:
	sudo apt-get install -y make build-essential pkg-config libsdl2-dev

clean: $(SUBDIRS)
	$(MAKE) -C battleship $@
	$(MAKE) -C celda $@
	$(MAKE) -C tetris $@
