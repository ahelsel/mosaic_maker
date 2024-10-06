//
// Created by Tony on 9/15/2024.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MOSAIC_MAKER_IMAGE_UTIL_H
#define MOSAIC_MAKER_IMAGE_UTIL_H

#include <stdlib.h>

typedef struct {
    unsigned char *pixels;
    unsigned width;
    unsigned height;
} Image;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

int loadImage(const char *filepath, Image *img);

void computeAverageColor(const Image *img, Color *avgColor);

void freeImage(Image *img);

#endif // MOSAIC_MAKER_IMAGE_UTIL_H

#ifdef __cplusplus
};
#endif