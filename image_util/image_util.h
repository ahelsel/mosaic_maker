//
// Created by Tony on 9/15/2024.
//

#ifndef MOSAIC_MAKER_IMAGE_UTIL_H
#define MOSAIC_MAKER_IMAGE_UTIL_H

#ifndef __cplusplus
extern "C" {
#endif

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

int load_image(const char* filename, Image* img);
int save_image(const char* filename, const Image* img);
int compute_average_color(const Image* img, int x, int y, int width, int height, Color* avg_color);
void free_image(Image* img);

#ifndef __cplusplus
}
#endif

#endif //MOSAIC_MAKER_IMAGE_UTIL_H
