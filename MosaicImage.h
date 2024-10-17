//
// Created by Tony Helsel on 10/17/24.
//

#ifndef MOSAIC_MAKER_MOSAICIMAGE_H
#define MOSAIC_MAKER_MOSAICIMAGE_H

#include <vector>
#include "lodepng/lodepng.h"
#include "image_util.h"

struct Tile {
    Image* image;
    Color avgColor;

    Tile(Image* img, const Color& color) : image(img), avgColor(color) {}
};

class MosaicImage {

private:
    std::vector<Image> tileImages;
    std::vector<Tile>  tiles;
    Image targetImage;

public:
    MosaicImage(const Image& target, const std::vector<Image>& images) : targetImage(target), tileImages(images) {};
    ~MosaicImage() = default;

    void createMosaic(unsigned numberOfTiles, unsigned pixelsPerTile);
    void saveMosaic(const char* outputImagePath);

// helpers
private:
    void loadTiles(const char* tileDirectoryPath);
    void computeTiles(unsigned pixelsPerTile);

    Color computeAverageColor(const Image& img);
    Tile& findMatchingTile(const Color& color);
    Color computeAverageColorOfRegion(const Image& img, int row, int col, int tile_width, int tile_height);
};

#endif //MOSAIC_MAKER_MOSAICIMAGE_H
