#pragma once

#include "clip/reformat.h"
#include "clip/output.h"
#include "clip/dictionary.h"
#include "clip/video_options.h"


namespace clip
{


static const int scaleFlag = SWS_BICUBIC;


class VideoOutput : public Output
{
public:
    VideoOutput(
        OutputContext &outputContext,
        Dictionary &codecOptions,
        VideoOptions &videoOptions)
        :
        Output(outputContext, outputContext->oformat->video_codec),
        options_(videoOptions)
    {
        codecOptions.Set(
            "preset",
            presetStrings[static_cast<size_t>(videoOptions.preset)]);

        if (videoOptions.qualityFactor >= 0)
        {
            codecOptions.Set(
                "crf",
                std::to_string(videoOptions.qualityFactor).c_str());
        }

        this->codecContext_->profile = videoOptions.profile;

        if (videoOptions.bitRate > 0)
        {
            this->codecContext_->bit_rate = videoOptions.bitRate;
        }

        // Resolution must be a multiple of two.
        assert(videoOptions.height % 2 == 0);
        assert(videoOptions.width % 2 == 0);

        this->codecContext_->height = videoOptions.height;
        this->codecContext_->width = videoOptions.width;

        /* timebase: This is the fundamental unit of time (in seconds) in
         * terms of which frame timestamps are represented. For fixed-fps
         * content, timebase should be 1/framerate and timestamp increments
         * should be identical to 1. */
        this->stream_->time_base = AVRational{1, videoOptions.framesPerSecond};
        this->codecContext_->time_base = this->stream_->time_base;

        this->timeStamp_ = clip::TimeStamp(0, this->codecContext_->time_base);

        this->codecContext_->gop_size = videoOptions.gopSize;

        this->codecContext_->pix_fmt = videoOptions.outPixelFormat;

        this->frame_ = Frame(
            videoOptions.outPixelFormat,
            videoOptions.height,
            videoOptions.width);

        if (videoOptions.outPixelFormat != videoOptions.inPixelFormat)
        {
            // The input and output formats do not match.
            // A Reformat instance and a temporary frame is needed.
            this->reformat = Reformat(
                this->codecContext_,
                videoOptions.inPixelFormat,
                scaleFlag);

            this->intermediate_ = Frame(
                videoOptions.inPixelFormat,
                videoOptions.height,
                videoOptions.width);
        }

        AVCodecContext *codecContext = this->codecContext_;

        int result = avcodec_open2(
            codecContext,
            this->codec_,
            codecOptions.Get());

        if (result < 0)
        {
            throw VideoError(
                DescribeError("Could not open video codec", result));
        }

        /* copy the stream parameters to the muxer */
        result = avcodec_parameters_from_context(
            this->stream_->codecpar,
            codecContext);

        if (result < 0)
        {
            throw VideoError("Could not copy the stream parameters");
        }
    }

    AVFrame * GetNextFrame()
    {
        // The encoder may still be using the last frame passed ot it.
        // Create a new frame if necessary.
        this->frame_.MakeWritable();

        if (this->options_.inPixelFormat != this->options_.outPixelFormat)
        {
            // The frame must be transcoded to the output format.
            // Return the input-formatted frame.
            return this->intermediate_;
        }
        else
        {
            return this->frame_;
        }
    }
    
    size_t GetStride() const
    {
        if (this->options_.inPixelFormat != this->options_.outPixelFormat)
        {
            // The frame must be transcoded to the output format.
            // Return the input-formatted frame.
            return static_cast<size_t>(this->intermediate_->linesize[0]);
        }
        else
        {
            return static_cast<size_t>(this->frame_->linesize[0]);
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

private:
    void FinishFrame_()
    {
        this->frame_->pts = this->timeStamp_.Count();
        ++this->timeStamp_;
        
        if (this->options_.inPixelFormat != this->options_.outPixelFormat)
        {
            // The frame must be transcoded to the output format.
            this->reformat(this->intermediate_, this->frame_);
        }
    }


private:
    VideoOptions options_;
    Reformat reformat;
    Frame frame_;
    Frame intermediate_;

    // Presentation time stamp of the next frame that will be generated.
    clip::TimeStamp timeStamp_;
};


enum class Motion: int
{
    low = 1,
    medium = 2,
    high = 3,
    extreme = 4
};


inline
int64_t EstimateBitRate(
    int height,
    int width,
    int frameRate,
    Motion profile)
{
    static constexpr double compressionFactor = 0.12;

    double estimate =
        height
        * width
        * frameRate
        * static_cast<double>(profile)
        * compressionFactor;

    return static_cast<int64_t>(estimate);
}


} // namespace clip
