/**
  * @file packet.h
  * 
  * @brief Wrapper around AVPacket.
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

#include <libavcodec/packet.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/error.h"
#include "clip/output_context.h"
#include "clip/time_stamp.h"


namespace clip
{


class OutputPacket
{
public:
    OutputPacket()
        : packet_(av_packet_alloc())
    {
        if (!this->packet_)
        {
            throw VideoError("Could not allocate AVPacket");
        }
    }

    ~OutputPacket()
    {
        if (this->packet_)
        {
            av_packet_free(&this->packet_);
        }
    }

    OutputPacket(OutputPacket &&other) : packet_(other.packet_)
    {
        other.packet_ = NULL;
    }

    OutputPacket &operator=(OutputPacket &&other)
    {
        if (this->packet_)
        {
            av_packet_free(&this->packet_);
        }

        this->packet_ = other.packet_;
        other.packet_ = NULL;

        return *this;
    }

    operator AVPacket * () const
    {
        return this->packet_;
    }

    AVPacket * operator->() const
    {
        return this->packet_;
    }

    operator bool ()
    {
        return (this->packet_ != NULL);
    }

private:
    AVPacket *packet_;
};


class InputPacket
{
public:
    InputPacket()
        :
        packet_(NULL)
    {

    }

    InputPacket(AVPacket *packet)
        :
        packet_(packet)
    {

    }

    ~InputPacket()
    {
        if (this->packet_)
        {
            av_packet_unref(this->packet_);
        }
    }

    InputPacket(const InputPacket &) = delete;

    InputPacket(InputPacket &&other) : packet_(other.packet_)
    {
        other.packet_ = NULL;
    }

    InputPacket & operator=(InputPacket &&other)
    {
        if (this->packet_)
        {
            av_packet_unref(this->packet_);
        }

        this->packet_ = other.packet_;
        other.packet_ = NULL;

        return *this;
    }

    InputPacket & operator=(const InputPacket &) = delete;

    operator AVPacket * () const
    {
        return this->packet_;
    }

    AVPacket * operator->() const
    {
        return this->packet_;
    }

    operator bool ()
    {
        return (this->packet_ != NULL);
    }

private:
    AVPacket *packet_;
};


void LogPacket(
    std::ostream &outputStream,
    const OutputContext &outputContext,
    const AVPacket *packet)
{
    auto timeBase = outputContext->streams[packet->stream_index]->time_base;

    TimeStamp pts(packet->pts, timeBase);
    TimeStamp dts(packet->dts, timeBase);
    TimeStamp duration(packet->duration, timeBase);

    outputStream
        << "pts: " << pts << " (" << Seconds(pts) << "), "
        << "dts: " << dts << " (" << Seconds(dts) << "), "
        << "duration: " << duration << " (" << Seconds(duration) << "), "
        << "stream: " << packet->stream_index << std::endl;
}


} // namespace clip
