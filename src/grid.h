#pragma once

#include <optional>
#include <vector>
#include <string>
#include <fstream>

struct Tile {
    unsigned int type;
};

struct Grid{
    unsigned int size_x;
    unsigned int size_y;
    //TODO: should probably be immutable, but reassignable

    std::vector<std::vector<Tile>> tiles;

    Grid(size_t width, size_t height);

    void Place(unsigned int x, unsigned int y, unsigned int type);

    Tile GetTile(unsigned int x, unsigned int y) const; //TODO: Maybe make this a reference idk

    void SaveToFile(std::string filename);

    static std::optional<Grid> LoadFromFile(std::string filename);

    static Grid NewDefault(unsigned int width, unsigned int height);
};
