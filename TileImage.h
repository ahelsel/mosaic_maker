//
// Created by Tony on 9/23/2024.
//

#ifndef MOSAIC_MAKER_TILEIMAGE_H
#define MOSAIC_MAKER_TILEIMAGE_H


#include "image_util/image_util.h"

class TileImage {
public:
    explicit TileImage(const std::string& filename);
    ~TileImage()
    void compute_average_color();

    Image img;
    Color avg_color;

private:
    TileImage() = delete;
};


#endif //MOSAIC_MAKER_TILEIMAGE_H
