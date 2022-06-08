/**
  * @file resample.h
  * 
  * @brief Convert audio samples between formats.
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

#include <libavutil/opt.h>
#include <libswresample/swresample.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/codec_context.h"
#include "clip/error.h"


namespace clip
{


template<typename InputOptions>
class Resample
{
  public:
    Resample() : context_(NULL) {}

    Resample(const CodecContext &outputCodec, const InputOptions &inputOptions)
    {
        this->context_ = swr_alloc();

        if (!this->context_)
        {
            throw VideoError("Could not allocate resampler context");
        }

        /* set options */
        av_opt_set_int(
            this->context_,
            "in_channel_count",
            inputOptions.channelLayout.GetChannelCount(),
            0);

        av_opt_set_int(
            this->context_,
            "in_sample_rate",
            inputOptions.sampleRate,
            0);

        av_opt_set_sample_fmt(
            this->context_,
            "in_sample_fmt",
            InputOptions::Format::value,
            0);

        av_opt_set_int(
            this->context_,
            "out_channel_count",
            outputCodec->channels,
            0);

        av_opt_set_int(
            this->context_,
            "out_sample_rate",
            outputCodec->sample_rate,
            0);

        av_opt_set_sample_fmt(
            this->context_,
            "out_sample_fmt",
            outputCodec->sample_fmt,
            0);

        /* initialize the resampling context */
        if (swr_init(this->context_) < 0)
        {
            std::runtime_error("Failed to initialize the resampling context");
        }
    }

    Resample(Resample &&other) : context_(other.context_)
    {
        other.context_ = NULL;
    }

    Resample &operator=(Resample &&other)
    {
        if (this->context_)
        {
            swr_free(&this->context_);
        }

        this->context_ = other.context_;
        other.context_ = NULL;

        return *this;
    }

    ~Resample()
    {
        if (this->context_)
        {
            swr_free(&this->context_);
        }
    }

    operator bool ()
    {
        return (this->context_ != NULL);
    }

    operator SwrContext * ()
    {
        return this->context_;
    }

    SwrContext * operator->()
    {
        return this->context_;
    }

  private:
    struct SwrContext *context_;
};


} // end namespace clip
