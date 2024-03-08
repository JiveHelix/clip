/**
  * @file codec.h
  *
  * @brief Wrapper around AVCodec.
  *
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/ffmpeg_shim.h"
FFMPEG_SHIM_PUSH_IGNORES
extern "C"
{

#include <libavcodec/codec.h>

}
FFMPEG_SHIM_POP_IGNORES


#include <iostream>
#include "clip/error.h"


namespace clip
{


class Codec
{
public:
    Codec(AVCodecID codecId)
        :
        codec_(avcodec_find_encoder(codecId))
    {
        if (!this->codec_)
        {
            std::cerr << "Could not find encoder for '"
                      << avcodec_get_name(codecId) << std::endl;

            throw VideoError("Could not find encoder.");
        }
    }

    // No destructor.
    // The AVCodec is managed by ffmpeg.
    // We didn't allocate it, we don't clean it up.

    operator const AVCodec * () const
    {
        return this->codec_;
    }

    const AVCodec * operator->()
    {
        return this->codec_;
    }

    /**
     ** @return True if sampleRate is explicitly supported, or if unknown.
     **/
    bool SupportsSampleRate(int sampleRate) const
    {
        if (!this->codec_->supported_samplerates)
        {
            // No samplerates are explicitly supported
            // Assuming true (only because ffmpeg sample code assumes true).
            return true;
        }

        // The codec specifies supported sample rates.
        const int *it = this->codec_->supported_samplerates;

        for (; *it; ++it)
        {
            if (*it == sampleRate)
            {
                return true;
            }
        }

        return false;
    }

    /**
     ** @return true if channelLayout is explicitly supported, or if unknown.
     **/
    bool SupportsChannelLayout(uint64_t channelLayout) const
    {
        if (!this->codec_->ch_layouts)
        {
            // Assuming true (only because ffmpeg sample code assumes true).
            return true;
        }

        const AVChannelLayout *it = this->codec_->ch_layouts;

        // ch_layouts is terminated with a zeroed layout.
        for (; it->u.mask && it->nb_channels; ++it)
        {
            if (it->u.mask == channelLayout)
            {
                return true;
            }
        }

        return false;
    }

private:
    const AVCodec *codec_;
};


} // end namespace clip
