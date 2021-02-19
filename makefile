CC=g++
CPPFLAGS=-O2 -std=c++17
typer: main.cpp
	$(CC) main.cpp -o typer $(CPPFLAGS)
clean:
	rm typer
