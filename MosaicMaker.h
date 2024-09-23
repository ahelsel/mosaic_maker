//
// Created by Tony on 9/23/2024.
//

#ifndef MOSAIC_MAKER_MOSAICMAKER_H
#define MOSAIC_MAKER_MOSAICMAKER_H

#include <vector>
#include <string>
#include "TileImage.h"
#include "image_util/image_util.h"

class MosaicMaker {
public:
    MosaicMaker(
            const std::string& target_image_filename,
            const std::vector<std::string>& tile_image_filenames,
            int tile_width,
            int tile_height
            );

    ~MosaicMaker();

    void create_mosaic(const std::string& output_filename);

private:
    Image target_image;
    std::vector<TileImage> tile_images;
    int tile_width;
    int tile_height;
    std::vector<std::vector<Color>> target_tile_colors;
    Image mosaic_image;

    // Helpers
    void load_target_image();
    void load_tile_images();
    void divide_target_image();
    void computer_target_tile_colors();

    const TileImage& find_best_match(const Color& target_color);
    void construct_mosaic_image();
    void save_mosaic_image(const std::string& filename);
};


#endif //MOSAIC_MAKER_MOSAICMAKER_H
