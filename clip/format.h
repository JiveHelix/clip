/**
  * @file format.h
  * 
  * @brief Defines supported AVOutputFormats.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


extern "C"
{

#include <libavformat/avformat.h>

}

#include "clip/error.h"


namespace clip
{


namespace format
{


template<typename Derived>
struct Format
{
    static const AVOutputFormat * Get()
    {
        const AVOutputFormat *result =
            av_guess_format(Derived::name, NULL, NULL);

        if (result == NULL)
        {
            throw ClipError("Format not found");
        }

        if (result->video_codec == AV_CODEC_ID_NONE)
        {
            throw ClipError(
                std::string(Derived::name) + " video codec not found");
        }

        if (result->audio_codec == AV_CODEC_ID_NONE)
        {
            throw ClipError(
                std::string(Derived::name) + " audio codec not found");
        }

        return result;
    }
};


struct Mov: Format<Mov>
{
    static constexpr const char *name = "mov";
    static constexpr const char *extension = "mov";
};


struct Mp4: Format<Mp4>
{
    static constexpr const char *name = "mp4";
    static constexpr const char *extension = "mp4";
};


struct Matroska: Format<Matroska>
{
    static constexpr const char *name = "matroska";
    static constexpr const char *extension = "mkv";
};


struct Ogg: Format<Ogg>
{
    static constexpr const char *name = "ogg";
    static constexpr const char *extension = "ogg";
};


} // end namespace format


} // end namespace clip
