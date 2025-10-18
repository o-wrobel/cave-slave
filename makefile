CC = g++
BUILD_DIR = build

BINARY = $(BUILD_DIR)/bin
CFILES = src/main.cpp #src/game.cpp
OBJECTS = main.o #game.o

WINDOWS_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
LINUX_FLAGS = -lraylib  -lGL -lm -lpthread #-lglfw3

INCLUDE = -I"C:\Libraries\raylib-5.5_win64_mingw-w64\include"
LIB = -L"C:\Libraries\raylib-5.5_win64_mingw-w64\lib"

all: $(BINARY)
# 	make run

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LINUX_FLAGS) $(INCLUDE) $(LIB)

%.o: src/%.cpp
	$(CC) -c -o $@ $^ $(INCLUDE) -Weffc++

run: $(BINARY)
	$(BINARY)

clean:
	rm -rf $(OBJECTS)
