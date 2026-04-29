FLAGS = -pedantic-errors -std=c++17

word_dungeon: main.cpp ui.cpp dungeon.cpp player.cpp puzzle_engine.cpp database.cpp ui.h dungeon.h player.h puzzle_engine.h database.h
	g++ $(FLAGS) main.cpp ui.cpp dungeon.cpp player.cpp puzzle_engine.cpp database.cpp -o word_dungeon.exe

clean:
	rm -f word_dungeon.exe

.PHONY: clean