/**
  * @file audio_output.h
  * 
  * @brief Creates an audio output tied to a stream.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include <limits>

#include "clip/channel_layout.h"
#include "clip/resample.h"
#include "clip/error.h"
#include "clip/output.h"
#include "clip/dictionary.h"
#include "clip/sample_format.h"


namespace clip
{


template<AVSampleFormat sampleFormat>
struct AudioOptions
{
    using Format = SampleFormat<sampleFormat>;
    int sampleRate;
    int bitRate;
    ChannelLayout channelLayout;

    void RequireCompatible(const Codec &codec) const
    {
        if (!codec.SupportsSampleRate(this->sampleRate))
        {
            throw AudioError("Sample rate not supported");
        }

        if (!codec.SupportsChannelLayout(this->channelLayout.Get()))
        {
            throw AudioError("Channel layout not supported");
        }
    }
};


template<typename Options>
class AudioOutput : public Output
{
public:
    AudioOutput(
        std::shared_ptr<OutputContext> outputContext,
        Dictionary &codecOptions,
        const Options &audioOptions)
        :
        Output(outputContext, (*outputContext)->oformat->audio_codec),
        options_(audioOptions)
    {
        if (this->codec_->type != AVMEDIA_TYPE_AUDIO)
        {
            throw AudioError("Not an audio codec.");
        }

        if (this->codec_->sample_fmts)
        {
            // Use the default sample format for this codec.
            this->codecContext_->sample_fmt = this->codec_->sample_fmts[0];
        }
        else
        {
            // The sample format is unknown.
            // Use planar floating-point.
            this->codecContext_->sample_fmt = AV_SAMPLE_FMT_FLTP;
        }

        this->codecContext_->bit_rate = audioOptions.bitRate;
        
        audioOptions.RequireCompatible(this->codec_);

        this->codecContext_->sample_rate = audioOptions.sampleRate;

        this->codecContext_->channel_layout = audioOptions.channelLayout.Get();

        this->codecContext_->channels =
            audioOptions.channelLayout.GetChannelCount();

        this->stream_->time_base = AVRational{1, audioOptions.sampleRate};

        this->codecContext_->time_base = this->stream_->time_base;

        this->timeStamp_ = TimeStamp(0, this->codecContext_->time_base);

        /* create resampler context */
        this->resample_ = Resample<Options>(this->codecContext_, audioOptions);

        AVCodecContext *codecContext = this->codecContext_;

        // AVCodecs must be explicitly opened, but they are only closed by the
        // owning AVCodecContext.
        int result = avcodec_open2(
            codecContext,
            this->codec_,
            codecOptions.Get());

        if (result < 0)
        {
            throw FfmpegError(
                "avcodec_open2 failed: " + AvErrorToString(result));
        }
        
        int sampleCount;

        if (this->codec_->capabilities 
                & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        {
            sampleCount = 10000;
        }
        else
        {
            sampleCount = codecContext->frame_size;
        }

        if (sampleCount <= 0)
        {
            throw AudioError("Frame size must be positive.");
        }
        
        this->sampleCount_ = static_cast<size_t>(sampleCount);

        this->frame_ = Frame(
            codecContext->sample_fmt,
            codecContext->channel_layout,
            codecContext->sample_rate,
            sampleCount);

        if (codecContext->sample_fmt != Options::Format::value)
        {
            this->intermediate_ = Frame(
                Options::Format::value,
                codecContext->channel_layout,
                codecContext->sample_rate,
                sampleCount);
        }

        /* copy the stream parameters to the muxer */
        result = avcodec_parameters_from_context(
            this->stream_->codecpar,
            codecContext);

        if (result < 0)
        {
            throw FfmpegError("Could not copy stream parameters.");
        }
    }

    AVFrame * GetNextFrame()
    {
        // The encoder may still be using the last frame passed ot it.
        // Create a new frame if necessary.
        this->frame_.MakeWritable();

        if (this->codecContext_->sample_fmt != Options::Format::value)
        {
            return this->intermediate_;
        }
        else
        {
            return this->frame_;
        }
    }

    /*
     * Write this class's frame.
     */
    void WriteFrame()
    {
        this->FinishFrame_();
        this->WriteFrame_(this->frame_);  
    }

    TimeStamp GetTimeStamp() const
    {
        return this->timeStamp_;
    }

    size_t GetSampleCount() const
    {
        return this->sampleCount_;
    }

    const Options & GetOptions() const
    {
        return this->options_;
    }

private:
    void FinishFrame_()
    {
        if (this->codecContext_->sample_fmt != Options::Format::value)
        {
            int sampleCount = static_cast<int>(this->sampleCount_);

            /* convert to destination format */
            int result = swr_convert(
                this->resample_,
                this->frame_->data,
                sampleCount,
                const_cast<const uint8_t **>(this->intermediate_->data),
                sampleCount);

            if (result < 0)
            {
                throw FfmpegError("Error while converting");
            }
        }
        
        this->frame_->pts = this->timeStamp_.Count();
        this->timeStamp_ += this->frame_->nb_samples;
    }

private:
    Options options_;
    Resample<Options> resample_;
    Frame frame_;
    Frame intermediate_;

    // Presentation time stamp of the next frame that will be generated.
    clip::TimeStamp timeStamp_;
    size_t sampleCount_;
};



} // end namespace clip
