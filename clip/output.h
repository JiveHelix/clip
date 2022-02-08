#pragma once


#include <string>


#include "clip/error.h"
#include "clip/output_context.h"
#include "clip/codec.h"
#include "clip/codec_context.h"
#include "clip/stream.h"
#include "clip/time_stamp.h"
#include "clip/frame.h"
#include "clip/packet.h"


namespace clip
{


CREATE_EXCEPTION(OutputError, VideoError);


class Output
{

public:
    void Flush()
    {
        while (0 == this->WriteFrame_(NULL))
        {
            // The encoder is still working through frames.
        };
    }


protected:
    Output(OutputContext &outputContext, AVCodecID codecId)
        : 
        outputContext_(outputContext),
        codec_(codecId),
        codecContext_(this->codec_),
        stream_(outputContext)
    {
        /* Some formats want stream headers to be separate. */
        if (this->outputContext_->oformat->flags & AVFMT_GLOBALHEADER)
        {
            this->codecContext_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }


    /*
     * encode one frame and send it to the muxer
     * return true when encoding is finished, 0 otherwise
     */
    bool WriteFrame_(AVFrame *source)
    {
        int result;

        // send the frame to the encoder
        result = avcodec_send_frame(this->codecContext_, source);

        if (result < 0)
        {
            throw OutputError(
                DescribeError("Error sending a frame to the encoder", result));
        }

        while (result >= 0)
        {
            result = avcodec_receive_packet(this->codecContext_, this->packet_);

            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF)
            {
                break;
            }
            else if (result < 0)
            {
                throw OutputError("Error receiving encoded packet");
            }

            // rescale output packet timestamp values from codec to stream
            // timebase
            av_packet_rescale_ts(
                this->packet_,
                this->codecContext_->time_base,
                this->stream_->time_base);

            this->packet_->stream_index = this->stream_->index;

#ifndef NDEBUG
            LogPacket(std::cout, this->outputContext_, this->packet_);
#endif

            // Write the compressed frame to the media file.
            result = av_interleaved_write_frame(
                this->outputContext_,
                this->packet_);

            /* packet is now blank (av_interleaved_write_frame() takes
             * ownership of its contents and resets packet), so that no
             * unreferencing is necessary.  This would be different if one used
             * av_write_frame(). */

            if (result < 0)
            {
                throw OutputError(
                    DescribeError("Error writing output packet", result));
            }
        }

        return (result == AVERROR_EOF);
    }


private:
    OutputContext &outputContext_;

protected:
    Codec codec_;

public:
    CodecContext codecContext_;

protected:
    Stream stream_;

private:
    Packet packet_;
};









} // namespace clip
