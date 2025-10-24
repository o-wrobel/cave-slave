INCLUDE = -I"C:\Libraries\raylib-5.5_win64_mingw-w64\include"
LIB = -L"C:\Libraries\raylib-5.5_win64_mingw-w64\lib"
BUILD_DIR = build
SRC_DIRS = . src

#########################################

CC = g++

BINARY = bin.exe
SRC_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJECTS = $(patsubst %.cpp,%.o,$(SRC_FILES))

WINDOWS_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
LINUX_FLAGS = -lraylib  -lGL -lm -lpthread
CFLAGS = $(LINUX_FLAGS) -Weffc++


all: $(BINARY)
	make run
	rm -rf $(OBJECTS)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIB)

%.o: src/%.cpp
	$(CC) -c -o $@ $^ $(INCLUDE)

run: $(BINARY)
	./$(BINARY)

clean:
	rm -rf $(OBJECTS)
