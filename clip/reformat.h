/**
  * @file reformat.h
  *
  * @brief Convert video to other formats.
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

#include <libswscale/swscale.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/frame.h"
#include "clip/codec_context.h"
#include "clip/error.h"


namespace clip
{


class Reformat
{
public:
    Reformat(): context_(NULL) {}

    Reformat(
        CodecContext &targetCodec,
        AVPixelFormat sourceFormat,
        int scaleFlag = SWS_BICUBIC)
    {
        this->context_ = sws_getContext(
            targetCodec->width,
            targetCodec->height,
            sourceFormat,
            targetCodec->width,
            targetCodec->height,
            targetCodec->pix_fmt,
            scaleFlag,
            NULL,
            NULL,
            NULL);

        if (!this->context_)
        {
            throw VideoError("Could not initialize SwsContext");
        }
    }

    Reformat(Reformat &&other)
        :
        context_(other.context_)
    {
        other.context_ = NULL; 
    }

    Reformat & operator=(Reformat &&other)
    {
        sws_freeContext(this->context_);
        this->context_ = other.context_;
        other.context_ = NULL;
        return *this;
    }

    ~Reformat()
    {
        // If context_ is NULL, then this call does nothing.
        // Also, this interface is inconsistent with the rest of ffmpeg. Most
        // *free functions take a pointer to a pointer so they can set the
        // freed pointer to NULL. This one just wants a pointer.
        sws_freeContext(this->context_);
    }

    operator struct SwsContext * ()
    {
        return this->context_;
    }

    operator bool ()
    {
        return (this->context_ != NULL);
    }

    int operator()(const Frame &source, Frame &target)
    {
        return sws_scale(
            this->context_,
            source->data,
            source->linesize,
            0,
            source->height,
            target->data,
            target->linesize);
    }

private:
    struct SwsContext *context_;
};


} // end namespace clip
