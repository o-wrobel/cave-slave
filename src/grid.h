#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <string>
#include <fstream>

struct Tile {
    uint16_t type;
};

struct Grid{
    uint16_t size_x;
    uint16_t size_y;
    //TODO: should probably be immutable, but reassignable

    std::vector<std::vector<Tile>> tiles;

    Grid(size_t width, size_t height);

    void Place(uint16_t x, uint16_t y, uint16_t type);

    Tile GetTile(uint16_t x, uint16_t y) const; //TODO: Maybe make this a reference idk

    void SaveToFile(std::string filename);

    static std::optional<Grid> LoadFromFile(std::string filename);

    static Grid NewDefault(uint16_t width, uint16_t height);
};
