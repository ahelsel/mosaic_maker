//
// Created by Tony on 9/15/2024.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include "lodepng/lodepng.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct {
    unsigned char* pixels;
    unsigned width;
    unsigned height;
} Image;

Image* loadImage(const char* filename);
Image* resizeImage(const Image* sourceImage, unsigned newWidth, unsigned newHeight);
void freeImage(Image* image);
Color computeAverageColor(const Image* image);
void getDirectoryFilePaths(const char* directoryPath, char*** filenames, int* filecount);

#endif // IMAGE_UTIL_H

#ifdef __cplusplus
}
#endif
