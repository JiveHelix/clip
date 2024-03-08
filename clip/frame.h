/**
  * @file frame.h
  * 
  * @brief Wrapper around AVFrame.
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

#include <libavformat/avformat.h>
#include <libavutil/frame.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/error.h"
#include "clip/channel_layout.h"


namespace clip
{


class Frame
{
public:
    Frame(): frame_(NULL) {}

    Frame(
        AVSampleFormat sampleFormat,
        uint64_t channelLayout,
        int sampleRate,
        int sampleCount)
    {
        this->frame_ = av_frame_alloc();

        if (!this->frame_)
        {
            throw VideoError("Error allocating an audio frame");
        }

        this->frame_->format = sampleFormat;

        detail::ChannelLayout layout(channelLayout);
        layout.MakeCopy(&this->frame_->ch_layout);
        this->frame_->sample_rate = sampleRate;
        this->frame_->nb_samples = sampleCount;

        if (sampleCount)
        {
            // A non-zero sample count has been requested.
            // Allocate the frame's buffers.
            if (av_frame_get_buffer(this->frame_, 0) < 0)
            {
                throw VideoError("Error allocating an audio buffer");
            }
        }
    }

    Frame(AVPixelFormat pixelFormat, int height, int width)
    {
        this->frame_ = av_frame_alloc();

        if (!this->frame_)
        {
            throw VideoError("Error allocating a picture frame");
        }

        this->frame_->format = pixelFormat;
        this->frame_->height = height;
        this->frame_->width  = width;

        /* allocate the buffers for the frame data */
        if (av_frame_get_buffer(this->frame_, 0) < 0)
        {
            throw VideoError("Could not allocate frame data.");
        }
    }

    ~Frame()
    {
        if (this->frame_)
        {
            av_frame_free(&this->frame_);
        }
    }

    Frame(const Frame &) = delete;

    Frame(Frame &&other)
        :
        frame_(other.frame_)
    {
        other.frame_ = NULL;
    }

    Frame &operator=(const Frame &) = delete;

    Frame &operator=(Frame &&other)
    {
        assert(this != &other);

        if (this->frame_)
        {
            av_frame_free(&this->frame_);
        }

        this->frame_ = other.frame_;
        other.frame_ = NULL;

        return *this;
    }

    operator AVFrame * ()
    {
        return this->frame_;
    }

    const AVFrame * operator->() const
    {
        return this->frame_;
    }

    AVFrame * operator->()
    {
        return this->frame_;
    }

    operator bool ()
    {
        return (this->frame_ != NULL);
    }

    Frame & MakeWritable()
    {
        // The encoder may still be using the last frame passed ot it.
        // Create a new frame if necessary.
        //
        // If a reference count as been incremented, this function makes a deep
        // copy of the frame. Otherwise, nothing happens.

        int result = av_frame_make_writable(this->frame_);

        if (result < 0)
        {
            throw VideoError("Failed to av_frame_make_writable");
        }

        return *this;
    }

private:
    AVFrame * frame_;
};




} // end namespace clip
