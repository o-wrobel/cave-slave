#include "grid.h"
#include <vector>

Grid::Grid(size_t width, size_t height) :
    size_x(width),
    size_y(height),
    tiles(height, std::vector<Tile>(width))
{

}

void Grid::Place(unsigned int x, unsigned int y, unsigned int type){
    if (0 <= x && x < size_x && 0 <= y && y < size_y){
        tiles.at(y).at(x).type = type;

    }
}

Tile Grid::GetTile(unsigned int x, unsigned int y) const { //TODO: Maybe make this a reference idk
    return tiles.at(y).at(x);
}


Grid Grid::NewDefault(unsigned int width, unsigned int height){
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
