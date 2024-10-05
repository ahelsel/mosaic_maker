#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <limits>

#include "image_util/image_util.h"
#include "image_util/lodepng.h"

namespace fs = std::filesystem;

struct Tile {
    std::string filepath;
    Color avgColor;
    Image image;
};

int main() {
    // Hardcoded paths
    std::string targetImagePath = "path/to/target_image.png";
    std::string tileDirectory = "path/to/tile_images";
    std::string outputImagePath = "output_mosaic.png";

    // Load target image
    Image targetImage;
    if (loadImage(targetImagePath.c_str(), &targetImage) != 0) {
        return 1;
    }

    // Calculate the dimensions of the tiles
    unsigned tileWidth = 32;  // Adjust as needed
    unsigned tileHeight = 32;

    // Load tile images
    std::vector<Tile> tiles;
    for (const auto& entry : fs::directory_iterator(tileDirectory)) {
        if (entry.path().extension() == ".png") {
            Tile tile;
            tile.filepath = entry.path().string();
            if (loadImage(tile.filepath.c_str(), &tile.image) != 0) {
                continue;
            }
            computeAverageColor(&tile.image, &tile.avgColor);
            tiles.push_back(tile);
        }
    }

    if (tiles.empty()) {
        std::cerr << "No tile images loaded." << std::endl;
        freeImage(&targetImage);
        return 1;
    }

    // Create the output image
    unsigned outputWidth = (targetImage.width / tileWidth) * tileWidth;
    unsigned outputHeight = (targetImage.height / tileHeight) * tileHeight;
    Image outputImage;
    outputImage.width = outputWidth;
    outputImage.height = outputHeight;
    outputImage.pixels = (unsigned char*)malloc(outputWidth * outputHeight * 4);

    // Process each region of the target image
    for (unsigned y = 0; y < outputHeight; y += tileHeight) {
        for (unsigned x = 0; x < outputWidth; x += tileWidth) {
            // Compute average color of the region
            Color regionAvgColor = {0, 0, 0};
            unsigned long r = 0, g = 0, b = 0;
            unsigned pixelCount = tileWidth * tileHeight;

            for (unsigned ty = 0; ty < tileHeight; ++ty) {
                for (unsigned tx = 0; tx < tileWidth; ++tx) {
                    unsigned idx = 4 * ((y + ty) * targetImage.width + (x + tx));
                    r += targetImage.pixels[idx];
                    g += targetImage.pixels[idx + 1];
                    b += targetImage.pixels[idx + 2];
                }
            }

            regionAvgColor.r = (unsigned char)(r / pixelCount);
            regionAvgColor.g = (unsigned char)(g / pixelCount);
            regionAvgColor.b = (unsigned char)(b / pixelCount);

            // Find the best matching tile
            Tile* bestTile = nullptr;
            unsigned minDifference = std::numeric_limits<unsigned>::max();

            for (auto& tile : tiles) {
                int dr = regionAvgColor.r - tile.avgColor.r;
                int dg = regionAvgColor.g - tile.avgColor.g;
                int db = regionAvgColor.b - tile.avgColor.b;
                unsigned diff = dr * dr + dg * dg + db * db;

                if (diff < minDifference) {
                    minDifference = diff;
                    bestTile = &tile;
                }
            }

            // Copy the best tile into the output image
            if (bestTile) {
                for (unsigned ty = 0; ty < tileHeight; ++ty) {
                    for (unsigned tx = 0; tx < tileWidth; ++tx) {
                        unsigned outIdx = 4 * ((y + ty) * outputImage.width + (x + tx));
                        unsigned tileIdx = 4 * (ty * bestTile->image.width + tx);

                        outputImage.pixels[outIdx] = bestTile->image.pixels[tileIdx];
                        outputImage.pixels[outIdx + 1] = bestTile->image.pixels[tileIdx + 1];
                        outputImage.pixels[outIdx + 2] = bestTile->image.pixels[tileIdx + 2];
                        outputImage.pixels[outIdx + 3] = 255; // Set alpha to opaque
                    }
                }
            }
        }
    }

    // Save the output image
    unsigned error = lodepng_encode32_file(outputImagePath.c_str(), outputImage.pixels, outputImage.width, outputImage.height);
    if (error) {
        std::cerr << "Error saving mosaic: " << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Mosaic saved to " << outputImagePath << std::endl;
    }

    // Free resources
    freeImage(&targetImage);
    free(outputImage.pixels);
    for (auto& tile : tiles) {
        freeImage(&tile.image);
    }

    return 0;
}
