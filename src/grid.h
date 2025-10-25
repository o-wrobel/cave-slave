#pragma once

#include <vector>

struct Tile {
    unsigned int type;
};

struct Grid{
    const unsigned int size_x;
    const unsigned int size_y;

    std::vector<std::vector<Tile>> tiles;

    Grid(size_t width, size_t height);

    void Place(unsigned int x, unsigned int y, unsigned int type);

    Tile GetTile(unsigned int x, unsigned int y) const; //TODO: Maybe make this a reference idk

    static Grid NewDefault(unsigned int width, unsigned int height);
};
