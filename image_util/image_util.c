//
// Created by Tony on 9/15/2024.
//

#include "image_util.h"

int loadImage(const char* filepath, unsigned char** image, unsigned* width, unsigned* height) {
    unsigned error = lodepng_decode32_file(image, width, height, filepath);
    if (error) {
        printf("Error loading image %s: %s\n", filepath, lodepng_error_text(error));
        return (int)error;
    }
    return 0;
}

void computeAverageColor(const unsigned char* image, unsigned width, unsigned height, unsigned* r, unsigned* g, unsigned* b) {
    *r = *g = *b = 0;
    unsigned pixelCount = width * height;

    for (unsigned i = 0; i < pixelCount * 4; i += 4) {
        *r += image[i];
        *g += image[i + 1];
        *b += image[i + 2];
    }

    *r /= pixelCount;
    *g /= pixelCount;
    *b /= pixelCount;
}

int saveImage(const char* filename, const Image* img) {
    return 0;
}


const char* findBestMatchingTile(const char** tilePaths, int tileCount, unsigned targetR, unsigned targetG, unsigned targetB) {
    const char* bestTile = NULL;
    unsigned min_difference = UINT_MAX;

    for (int i = 0; i < tileCount; i++) {
        unsigned char* tileImage;
        unsigned tileWidth, tileHeight;

        if (loadImage(tilePaths[i], &tileImage, &tileWidth, &tileHeight) != 0) {
            continue;
        }

        unsigned tileR, tileG, tileB;
        computeAverageColor(tileImage, tileWidth, tileHeight, &tileR, &tileG, &tileB);

        unsigned diff = pow(targetR - tileR, 2) + pow(targetG - tileG, 2) + pow(targetB - tileB, 2);

        if (diff < min_difference) {
            min_difference = diff;
            bestTile = tilePaths[i];
        }

        free(tileImage);
    }
    return bestTile;
}