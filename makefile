CC = g++
BUILD_DIR = build

BINARY = $(BUILD_DIR)\bin
CFILES = src\main.cpp src\game.cpp
OBJECTS = main.o game.o

LFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
INCLUDE = -I"C:\Libraries\raylib-5.5_win64_mingw-w64\include"
LIB = -L"C:\Libraries\raylib-5.5_win64_mingw-w64\lib"

all: $(BINARY)
# 	make run

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LFLAGS) $(INCLUDE) $(LIB) 

%.o: src/%.cpp
	$(CC) -c -o $@ $^ $(INCLUDE) -Weffc++ 

run: $(BINARY)
	$(BINARY)	

clean:
	rm -rf $(OBJECTS)