/**
  * @file stream.h
  * 
  * @brief Wrapper around AVStream.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


extern "C"
{

#include <libavformat/avformat.h>

}

#include <cassert>
#include "clip/output_context.h"
#include "clip/error.h"


namespace clip
{


class Stream
{
public:
    Stream(OutputContext &outputContext)
        :
        avStream_(avformat_new_stream(outputContext, NULL))
    {

        if (!this->avStream_)
        {
            throw std::runtime_error("Could not allocate stream");
        }

        assert(outputContext->nb_streams > 0);

        this->avStream_->id = static_cast<int>(outputContext->nb_streams - 1);
    }

    // No destructor.
    // AVFormatContext owns the stream returned by avformat_new_stream
    // The documentation is wrong!
    // The user is NOT required to call avformat_free_context on the returned
    // AVStream *.
    // The user IS required to call avformat_free_context on the
    // AVFormatContext that this stream was created on.
 
    operator const AVStream * () const
    {
        return this->avStream_;
    }

    const AVStream * operator->() const
    {
        return this->avStream_;
    }

    operator AVStream * ()
    {
        return this->avStream_;
    }

    AVStream * operator->()
    {
        return this->avStream_;
    }

private:
    AVStream *avStream_;
};


} // namespace clip


