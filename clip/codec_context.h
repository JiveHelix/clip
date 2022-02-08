#pragma once

extern "C"
{

#include <libavcodec/avcodec.h>

}

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

