#include "grid.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>


Grid::Grid(size_t width, size_t height) :
    size_x(width),
    size_y(height),
    tiles(height, std::vector<Tile>(width))
{

}

void Grid::Place(uint16_t x, uint16_t y, uint16_t type){
    if (0 <= x && x < size_x && 0 <= y && y < size_y){
        tiles.at(y).at(x).type = type;

    }

}

Tile Grid::GetTile(uint16_t x, uint16_t y) const { //TODO: Maybe make this a reference idk
    return tiles.at(y).at(x);

}


Grid Grid::NewDefault(uint16_t width, uint16_t height){
    Grid grid(width, height);
    for (int y = 0; y < height; y++)
    {
        grid.Place(0, y, 6);
    }
    for (int x = 0; x < width; x++)
    {
        grid.Place(x, 0, 6);
    }
    for (int y = 0; y < height; y++)
    {
        grid.Place(width - 1, y, 6);
    }
    for (int x = 0; x < width; x++)
    {
        grid.Place(x, height - 1, 6);
    }

    return grid;

}

void Grid::SaveToFile(std::string filename){
    using Json = nlohmann::json;

    std::ofstream file("levels/" + filename + ".json");

    Json level_data;
    level_data["width"] = size_x;
    level_data["height"] = size_y;

    std::vector<std::vector<uint16_t>> tiles_data;
    for (const auto& row : tiles){
        std::vector<uint16_t> type_row;
        for (const auto& tile : row){
            type_row.push_back(tile.type);
        }
        tiles_data.push_back(type_row);
    }
    level_data["tiles"] = tiles_data;

    std::string json_string = level_data.dump(4); //TODO: make this cleaner somehow, whole file is 4000+ lines long

    if (file.is_open()){
        file << json_string;
        file.close();
    }

}

std::optional<Grid> Grid::LoadFromFile(std::string filename){
    try {
    using Json = nlohmann::json;

    std::ifstream file("levels/" + filename + ".json");

    Json level_data;
    file >> level_data;

    Grid return_grid(level_data["width"], level_data["height"]);

    std::vector<std::vector<uint16_t>> tiles_data = level_data["tiles"];
    for (int y = 0; y < return_grid.size_y; y++){
        for (int x = 0; x < return_grid.size_x; x++){
            return_grid.tiles[y][x].type = tiles_data[y][x];
        }
    }

    return return_grid;
    } catch (const std::exception& e) {
        std::cout << "Error loading grid from file: " << e.what() << std::endl;
        return std::nullopt;
    }
}
