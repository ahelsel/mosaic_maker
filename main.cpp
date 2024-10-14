#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

#include <cstdlib>
#include <cstring>

#include "image_util.h"

extern "C" {
#include "lodepng/lodepng.h"
}

// hardcoded absolute paths for target image, tile directories, and output images
// we need to access from the previous directory because the exe is executing in the cmake-build-debug directory
#define TARGET_IMAGE_PATH   "../target_images//mona_lisa.png"
#define TILE_DIRECTORY      "../blocks"
#define OUTPUT_IMAGE_PATH   "../output_mosaic.png"

#define NUMBER_OF_TILES     100  // Number of tiles along the shorter dimension
#define PIXELS_PER_TILE     50   // Width/height of each tile in the mosaic

// Structure to hold tile information
struct Tile {
    Image* image;
    Color avgColor;
};


int local_init() {
    // TODO: local init function
    // error checking along the way
    return 0;
}

int local_cleanup(Image* targetImage, Image& outputImage,std::vector<Tile>& tiles) {
    // todo: error checking at each step
    freeImage(targetImage);
    free(outputImage.pixels);
    for (auto& tile : tiles) {
        freeImage(tile.image);
    }
    return 0;
}

// todo: error checking at each step
// resize the tile images and get the average color of each
int computeTiles(std::vector<Tile>& tiles, char** tileFilePaths, int tileFileCount) {
    for (int i = 0; i < tileFileCount; ++i) {
        Image* tileImage = loadImage(tileFilePaths[i]);
        if (tileImage) {
            // Resize tile image to PIXELS_PER_TILE x PIXELS_PER_TILE
            Image* resizedTileImage = resizeImage(tileImage, PIXELS_PER_TILE, PIXELS_PER_TILE);
            freeImage(tileImage); // Free the original tile image
            if (!resizedTileImage) {
                std::cerr << "Failed to resize tile image: " << tileFilePaths[i] << std::endl;
                continue;
            }
            Color avgColor = computeAverageColor(resizedTileImage);
            tiles.push_back({resizedTileImage, avgColor});
        } else {
            std::cerr << "Failed to load tile image: " << tileFilePaths[i] << std::endl;
        }
        free(tileFilePaths[i]);
    }
    free(tileFilePaths);
    return 0;
}

int main() {

    // Load target image
    const char* path = TARGET_IMAGE_PATH;
    Image* targetImage = loadImage(path);
    if (!targetImage) {
        std::cerr << "Failed to load target image." << std::endl;
        return EXIT_FAILURE;
    }

    // Load tile images
    char** tileFilePaths = nullptr;
    int tileFileCount = 0;
    getDirectoryFilePaths(TILE_DIRECTORY, &tileFilePaths, &tileFileCount);

    if (tileFileCount == 0) {
        std::cerr << "No tile images found in directory." << std::endl;
        freeImage(targetImage);
        return EXIT_FAILURE;
    }

    // todo: seperate function for computing tile specs; something like int computeTiles(std::vector<Tile>& tiles, char** tileFilePaths);
    // resize the tile images and get the average color of each
    std::vector<Tile> tiles;
    if (computeTiles(tiles, tileFilePaths, tileFileCount) != 0) {
        std::cerr << "Failed to resize tile images or compute average tile color." << std::endl;
        free(tileFilePaths);
        return 1;
    }

    std::cout << "Loaded " << tiles.size() << " tile images." << std::endl;

    // Determine the number of tiles along width and height
    unsigned tilesAlongWidth, tilesAlongHeight;

    if (targetImage->width <= targetImage->height) {
        tilesAlongWidth = NUMBER_OF_TILES;
        tilesAlongHeight = static_cast<unsigned>(std::round((double)targetImage->height / targetImage->width * NUMBER_OF_TILES));
    } else {
        tilesAlongHeight = NUMBER_OF_TILES;
        tilesAlongWidth = static_cast<unsigned>(std::round((double)targetImage->width / targetImage->height * NUMBER_OF_TILES));
    }

    // Output image dimensions
    unsigned outputWidth  = tilesAlongWidth  * PIXELS_PER_TILE;
    unsigned outputHeight = tilesAlongHeight * PIXELS_PER_TILE;

    // Calculate scaling factors
    double scaleX = (double)targetImage->width  / tilesAlongWidth;
    double scaleY = (double)targetImage->height / tilesAlongHeight;

    // Allocate output image memory
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

    // For each region ("tile") of the image, calculate the average color
    for (unsigned tileY = 0; tileY < tilesAlongHeight; ++tileY) {
        for (unsigned tileX = 0; tileX < tilesAlongWidth; ++tileX) {
            // Compute the region in the target image
            unsigned xStart = static_cast<unsigned>(tileX * scaleX);
            unsigned yStart = static_cast<unsigned>(tileY * scaleY);
            unsigned xEnd = static_cast<unsigned>((tileX + 1) * scaleX);
            unsigned yEnd = static_cast<unsigned>((tileY + 1) * scaleY);

            if (xEnd > targetImage->width) xEnd = targetImage->width;
            if (yEnd > targetImage->height) yEnd = targetImage->height;

            // Compute average color of the region
            Color regionAvgColor = {0, 0, 0};
            unsigned long r = 0, g = 0, b = 0;
            unsigned pixelCount = 0;

            for (unsigned y = yStart; y < yEnd; ++y) {
                for (unsigned x = xStart; x < xEnd; ++x) {
                    unsigned idx = 4 * (y * targetImage->width + x);
                    r += targetImage->pixels[idx];
                    g += targetImage->pixels[idx + 1];
                    b += targetImage->pixels[idx + 2];
                    ++pixelCount;
                }
            }

            if (pixelCount > 0) {
                regionAvgColor.r = static_cast<unsigned char>(r / pixelCount);
                regionAvgColor.g = static_cast<unsigned char>(g / pixelCount);
                regionAvgColor.b = static_cast<unsigned char>(b / pixelCount);
            }

            // Find the best matching tile image based on avg color
            Tile* bestTile = nullptr;
            unsigned minDifference = std::numeric_limits<unsigned>::max();

            for (auto& tile : tiles) {
                int dr = static_cast<int>(regionAvgColor.r) - static_cast<int>(tile.avgColor.r);
                int dg = static_cast<int>(regionAvgColor.g) - static_cast<int>(tile.avgColor.g);
                int db = static_cast<int>(regionAvgColor.b) - static_cast<int>(tile.avgColor.b);
                unsigned diff = dr * dr + dg * dg + db * db;

                if (diff < minDifference) {
                    minDifference = diff;
                    bestTile = &tile;
                }
            }

            // Copy the best tile into the output image
            if (bestTile) {
                // Output image coordinates
                unsigned outX = tileX * PIXELS_PER_TILE;
                unsigned outY = tileY * PIXELS_PER_TILE;

                for (unsigned ty = 0; ty < PIXELS_PER_TILE; ++ty) {
                    for (unsigned tx = 0; tx < PIXELS_PER_TILE; ++tx) {
                        unsigned outIdx = 4 * ((outY + ty) * outputImage.width + (outX + tx));
                        unsigned tileIdx = 4 * (ty * bestTile->image->width + tx);

                        outputImage.pixels[outIdx]     = bestTile->image->pixels[tileIdx];
                        outputImage.pixels[outIdx + 1] = bestTile->image->pixels[tileIdx + 1];
                        outputImage.pixels[outIdx + 2] = bestTile->image->pixels[tileIdx + 2];
                        outputImage.pixels[outIdx + 3] = 255; // Opaque
                    }
                }
            }
        }
        std::cout << "Processed row " << tileY + 1 << " of " << tilesAlongHeight << "." << std::endl;
    }


    std::cout << "Saving output image..." << std::endl;

    // Save the output image
    unsigned error = lodepng_encode32_file(OUTPUT_IMAGE_PATH, outputImage.pixels, outputImage.width, outputImage.height);
    if (error) {
        std::cerr << "Error saving mosaic: " << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Mosaic saved to " << OUTPUT_IMAGE_PATH << std::endl;
    }

    if (local_cleanup(targetImage, outputImage, tiles) != 0) {
        // print error
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
