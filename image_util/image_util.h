//
// Created by Tony on 9/15/2024.
//

#ifndef MOSAIC_MAKER_IMAGE_UTIL_H
#define MOSAIC_MAKER_IMAGE_UTIL_H

#include "lodepng.h"
#include "stdio.h"

#include <math.h>
#include <limits.h>

typedef struct {
    unsigned char* pixels;
    unsigned width;
    unsigned height;
} Image;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

int loadImage(const char* filepath, unsigned char** image, unsigned* width, unsigned* height);
void computeAverageColor(const unsigned char* image, unsigned width, unsigned height, unsigned* r, unsigned* g, unsigned* b);
int saveImage(const char* filename, const Image* img);

const char* findBestMatchingTile(const char** tilePaths, int tileCount, unsigned targetR, unsigned targetG, unsigned targetB);

#endif //MOSAIC_MAKER_IMAGE_UTIL_H
