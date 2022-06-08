/**
  * @file codec_context.h
  * 
  * @brief Wrapper around AVCodecContext.
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

#include <libavcodec/avcodec.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/error.h"


namespace clip
{

class CodecContext
{
public:
    CodecContext(const AVCodec *codec)
    {
        this->context_ = avcodec_alloc_context3(codec);

        if (!this->context_)
        {
            throw VideoError("Failed to allocate AVCodecContext");
        }
    }

    ~CodecContext()
    {
        if (this->context_)
        {
            avcodec_free_context(&this->context_);
        }
    }

    operator AVCodecContext * () const
    {
        return this->context_;
    }

    AVCodecContext * operator->() const
    {
        return this->context_;
    }

private:
    AVCodecContext *context_;
};

} // end namespace clip

