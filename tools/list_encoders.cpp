/**
  * @file list_encoders.cpp
  *
  * @brief Show installed encoders.
  *
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

extern "C"
{

#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>

}

#include <iostream>


int main()
{
    const AVCodec *avCodec;
    void *iterator = 0;

    while ((avCodec = av_codec_iterate(&iterator)))
    {
        if (av_codec_is_encoder(avCodec)
                && (avCodec->type == AVMEDIA_TYPE_VIDEO))
        {
            std::cout << avCodec->name << std::endl;
        }
    }

    const AVCodec *p = avcodec_find_encoder_by_name("libx264");

    if (!p)
    {
        std::cerr << "Cannot find libx264" << std::endl;
        return 1;
    }

    return 0;
}
