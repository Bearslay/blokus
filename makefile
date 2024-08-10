debug:
	@mkdir bin -p
	@mkdir bin/debug -p
	@g++ -c src/main.cpp -std=c++17 -m64 -g -Wall -I blokus -I btils -I bengine
	@g++ main.o -o bin/debug/blokus-debug -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./bin/debug/blokus-debug
release:
	@mkdir bin -p
	@mkdir bin/release -p
	@g++ -c src/main.cpp -std=c++17 -m64 -O3 -Wall -I blokus -I btils -I bengine
	@g++ main.o -o bin/release/blokus -s -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./bin/release/blokus
polymaker:
	@mkdir bin -p
	@mkdir bin/debug -p
	@g++ -c src/polyominoMaker.cpp -std=c++17 -m64 -g -Wall -I blokus -I btils -I bengine
	@g++ polyominoMaker.o -o bin/debug/polyominoMaker -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./bin/debug/polyominoMaker
