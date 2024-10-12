//
// Created by Tony on 9/15/2024.
//
#include "image_util.h"
#include <string.h>

Image* loadImage(const char* filename) {
    Image* image = (Image*)malloc(sizeof(Image));
    if (!image) {
        printf("Failed to allocate memory for image\n");
        return NULL;
    }
    unsigned error = lodepng_decode32_file(&image->pixels, &image->width, &image->height, filename);
    if (error) {
        printf("Error %u: %s\n", error, lodepng_error_text(error));
        free(image);
        return NULL;
    }
    return image;
}

void freeImage(Image* image) {
    if (image) {
        free(image->pixels);
        free(image);
    }
}

Color computeAverageColor(const Image* image) {
    Color avgColor = {0, 0, 0};
    unsigned long r = 0, g = 0, b = 0;
    unsigned pixelCount = image->width * image->height;

    for (unsigned i = 0; i < pixelCount; ++i) {
        r += image->pixels[4 * i];
        g += image->pixels[4 * i + 1];
        b += image->pixels[4 * i + 2];
    }

    avgColor.r = (unsigned char)(r / pixelCount);
    avgColor.g = (unsigned char)(g / pixelCount);
    avgColor.b = (unsigned char)(b / pixelCount);

    return avgColor;
}

void getDirectoryFilePaths(const char* directoryPath, char*** filenames, int* filecount) {
    DIR* directory = opendir(directoryPath);
    if (!directory) {
        printf("Failed to open directory at path: %s\n", directoryPath);
        *filenames = NULL;
        *filecount = 0;
        return;
    }

    struct dirent* entry;
    int count = 0;
    int capacity = 10;
    char** files = (char**)malloc(capacity * sizeof(char*));
    if (!files) {
        printf("Failed to allocate memory for filenames\n");
        closedir(directory);
        *filenames = NULL;
        *filecount = 0;
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        int nameLength = strlen(entry->d_name);
        if (nameLength > 4 && strcmp(entry->d_name + nameLength - 4, ".png") == 0) {
            if (count >= capacity) {
                capacity *= 2;
                files = (char**)realloc(files, capacity * sizeof(char*));
                if (!files) {
                    printf("Failed to reallocate memory for filenames\n");
                    closedir(directory);
                    *filenames = NULL;
                    *filecount = 0;
                    return;
                }
            }
            char* filepath = (char*)malloc(strlen(directoryPath) + nameLength + 2);
            if (!filepath) {
                printf("Failed to allocate memory for filepath\n");
                closedir(directory);
                *filenames = NULL;
                *filecount = 0;
                return;
            }
            strcpy(filepath, directoryPath);
            if (directoryPath[strlen(directoryPath) - 1] != '/') {
                strcat(filepath, "/");
            }
            strcat(filepath, entry->d_name);
            files[count++] = filepath;
        }
    }

    closedir(directory);
    *filenames = files;
    *filecount = count;
}

Image* resizeImage(const Image* sourceImage, unsigned newWidth, unsigned newHeight) {
    Image* resizedImage = (Image*)malloc(sizeof(Image));
    if (!resizedImage) {
        printf("Failed to allocate memory for resized image\n");
        return NULL;
    }
    resizedImage->width = newWidth;
    resizedImage->height = newHeight;
    resizedImage->pixels = (unsigned char*)malloc(newWidth * newHeight * 4);
    if (!resizedImage->pixels) {
        printf("Failed to allocate memory for resized image pixels\n");
        free(resizedImage);
        return NULL;
    }

    double scaleX = (double)sourceImage->width / newWidth;
    double scaleY = (double)sourceImage->height / newHeight;

    for (unsigned y = 0; y < newHeight; ++y) {
        for (unsigned x = 0; x < newWidth; ++x) {
            unsigned srcX = (unsigned)(x * scaleX);
            unsigned srcY = (unsigned)(y * scaleY);
            if (srcX >= sourceImage->width) srcX = sourceImage->width - 1;
            if (srcY >= sourceImage->height) srcY = sourceImage->height - 1;

            unsigned destIdx = 4 * (y * newWidth + x);
            unsigned srcIdx = 4 * (srcY * sourceImage->width + srcX);

            resizedImage->pixels[destIdx]     = sourceImage->pixels[srcIdx];
            resizedImage->pixels[destIdx + 1] = sourceImage->pixels[srcIdx + 1];
            resizedImage->pixels[destIdx + 2] = sourceImage->pixels[srcIdx + 2];
            resizedImage->pixels[destIdx + 3] = sourceImage->pixels[srcIdx + 3];
        }
    }
    return resizedImage;
}
