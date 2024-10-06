#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <limits>
#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include "image_util.h"

extern "C" {
#include "lodepng/lodepng.h"
}

// Structure to hold tile information
struct Tile {
    Image* image;
    Color avgColor;
};

int main() {
    // Paths
    std::string targetImagePath  = "mona_lisa.png";
    std::string tileImageDirPath = "blocks/";
    std::string outputImagePath  = "output_mosaic.png";

    // Load target image
    Image* targetImage = loadImage(targetImagePath.c_str());
    if (!targetImage) {
        std::cerr << "Failed to load target image." << std::endl;
        return EXIT_FAILURE;
    }

    // Load tile images
    char** tileFilePaths = nullptr;
    int tileFileCount = 0;
    getDirectoryFilePaths(tileImageDirPath.c_str(), &tileFilePaths, &tileFileCount);

    if (tileFileCount == 0) {
        std::cerr << "No tile images found in directory." << std::endl;
        freeImage(targetImage);
        return EXIT_FAILURE;
    }

    std::vector<Tile> tiles;
    for (int i = 0; i < tileFileCount; ++i) {
        Image* tileImage = loadImage(tileFilePaths[i]);
        if (tileImage) {
            Color avgColor = computeAverageColor(tileImage);
            tiles.push_back({tileImage, avgColor});
        } else {
            std::cerr << "Failed to load tile image: " << tileFilePaths[i] << std::endl;
        }
        free(tileFilePaths[i]);
    }
    free(tileFilePaths);

    std::cout << "Loaded " << tiles.size() << " tile images." << std::endl;

    // Determine tile dimensions
    unsigned tileWidth = tiles[0].image->width;
    unsigned tileHeight = tiles[0].image->height;

    // Calculate output image dimensions
    unsigned outputWidth = (targetImage->width / tileWidth) * tileWidth;
    unsigned outputHeight = (targetImage->height / tileHeight) * tileHeight;

    // Create output image
    unsigned char* outputPixels = (unsigned char*)malloc(outputWidth * outputHeight * 4);
    if (!outputPixels) {
        std::cerr << "Failed to allocate memory for output image." << std::endl;
        freeImage(targetImage);
        for (auto& tile : tiles) {
            freeImage(tile.image);
        }
        return EXIT_FAILURE;
    }
    Image outputImage = {outputPixels, outputWidth, outputHeight};

    // Process each region of the target image
    for (unsigned y = 0; y < outputHeight; y += tileHeight) {
        for (unsigned x = 0; x < outputWidth; x += tileWidth) {
            // Compute average color of the region
            Color regionAvgColor = {0, 0, 0};
            unsigned long r = 0, g = 0, b = 0;
            unsigned pixelCount = tileWidth * tileHeight;

            for (unsigned ty = 0; ty < tileHeight; ++ty) {
                for (unsigned tx = 0; tx < tileWidth; ++tx) {
                    unsigned idx = 4 * ((y + ty) * targetImage->width + (x + tx));
                    r += targetImage->pixels[idx];
                    g += targetImage->pixels[idx + 1];
                    b += targetImage->pixels[idx + 2];
                }
            }

            regionAvgColor.r = (unsigned char)(r / pixelCount);
            regionAvgColor.g = (unsigned char)(g / pixelCount);
            regionAvgColor.b = (unsigned char)(b / pixelCount);

            // Find the best matching tile
            Tile* bestTile = nullptr;
            unsigned minDifference = std::numeric_limits<unsigned>::max();

            for (auto& tile : tiles) {
                int dr = (int)regionAvgColor.r - (int)tile.avgColor.r;
                int dg = (int)regionAvgColor.g - (int)tile.avgColor.g;
                int db = (int)regionAvgColor.b - (int)tile.avgColor.b;
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
                        unsigned tileIdx = 4 * (ty * bestTile->image->width + tx);

                        outputImage.pixels[outIdx]     = bestTile->image->pixels[tileIdx];
                        outputImage.pixels[outIdx + 1] = bestTile->image->pixels[tileIdx + 1];
                        outputImage.pixels[outIdx + 2] = bestTile->image->pixels[tileIdx + 2];
                        outputImage.pixels[outIdx + 3] = 255; // Opaque
                    }
                }
            }
        }
        std::cout << "Processed row " << y / tileHeight + 1 << " of " << outputHeight / tileHeight << "." << std::endl;
    }

    // Save the output image
    unsigned error = lodepng_encode32_file(outputImagePath.c_str(), outputImage.pixels, outputImage.width, outputImage.height);
    if (error) {
        std::cerr << "Error saving mosaic: " << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Mosaic saved to " << outputImagePath << std::endl;
    }

    // Free resources
    freeImage(targetImage);
    free(outputImage.pixels);
    for (auto& tile : tiles) {
        freeImage(tile.image);
    }

    return EXIT_SUCCESS;
}
