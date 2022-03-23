/**
  * @file lossless_video.cpp
  * 
  * @brief Generates a lossless mkv.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#include <cstdlib>
#include <iostream>

#include "demo/create_video.h"
#include "clip/circle_gradient.h"
#include "clip/video_output.h"


static const clip::TimeStamp streamDuration(15);


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return EXIT_FAILURE;
    }

    std::string baseName(argv[1]);

    try
    {
        CreateVideo
        <
            clip::format::Matroska,
            clip::CircleGradientColors<uint16_t>
        >(
            baseName,
            clip::VideoOptions::MakeLossless(),
            streamDuration);
    }
    catch (clip::ClipError &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
