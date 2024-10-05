//
// Created by Tony on 9/15/2024.
//

#include "image_util.h"
#include "lodepng.h"
#include <stdio.h>

int loadImage(const char* filepath, Image* img) {
    unsigned error = lodepng_decode32_file(&img->pixels, &img->width, &img->height, filepath);
    if (error) {
        printf("Error loading image %s: %s\n", filepath, lodepng_error_text(error));
        return (int)error;
    }
    return 0;
}

void computeAverageColor(const Image* img, Color* avgColor) {
    unsigned long r = 0, g = 0, b = 0;
    unsigned pixelCount = img->width * img->height;

    for (unsigned i = 0; i < pixelCount * 4; i += 4) {
        r += img->pixels[i];
        g += img->pixels[i + 1];
        b += img->pixels[i + 2];
    }

    avgColor->r = (unsigned char)(r / pixelCount);
    avgColor->g = (unsigned char)(g / pixelCount);
    avgColor->b = (unsigned char)(b / pixelCount);
}

void freeImage(Image* img) {
    if (img->pixels) {
        free(img->pixels);
        img->pixels = NULL;
    }
}