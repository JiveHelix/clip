/**
  * @file output_context.h
  * 
  * @brief Wrapper around AVFormatContext.
  *     (Renamed to OutputContext to better reflect its usage.)
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

#include <string>
#include <iostream>
#include "clip/error.h"
#include "clip/dictionary.h"


#ifdef ff_const59
// No longer part of ffmpeg, but it was still there in version 4.4
#define FF_CONST_CAST(type, arg) const_cast<ff_const59 type *>(arg)
#else
#define FF_CONST_CAST(type, arg) arg
#endif


namespace clip
{


class OutputContext
{
private:
    class Context
    { 
    public:
        Context(const AVOutputFormat *outputFormat)
            :
            context_(NULL)
        {
            /* allocate the output media context */
            avformat_alloc_output_context2(
                &this->context_,
                FF_CONST_CAST(AVOutputFormat, outputFormat),
                NULL,
                NULL);

            if (!this->context_)
            {
                throw VideoError("Failed to allocate AVFormatContext");
            }
        }

        Context(Context &&other)
            :
            context_(other.context_)
        {
            other.context_ = NULL;
        }

        Context(const Context &) = delete;

        Context & operator=(Context &&other)
        {
            assert(this != &other);

            if (this->context_)
            {
                avformat_free_context(this->context_);
            }

            this->context_ = other.context_;
            other.context_ = NULL;

            return *this;
        }

        ~Context()
        {
            if (!this->context_)
            {
                return;
            }

            avformat_free_context(this->context_);
        }

        AVFormatContext * Get() const
        {
            return this->context_;
        }

    private:
        AVFormatContext *context_;
    };


public:
    OutputContext(
        const AVOutputFormat *outputFormat,
        const std::string &fileName)
        :
        context_(outputFormat),
        isInitialized_(false),
        isFinalized_(false)
    {
        if (!(outputFormat->flags & AVFMT_NOFILE))
        {
            // We need a file.
            int result = avio_open(
                &this->context_.Get()->pb,
                fileName.c_str(),
                AVIO_FLAG_WRITE);

            if (result < 0)
            {
                throw VideoError(
                    DescribeError("Failed to open output file.", result));
            }
        }
    }

    ~OutputContext()
    {
        if (!this->isFinalized_)
        {
            try
            {
                this->Finalize();
            }
            catch (ClipError &error)
            {
                // Do not propagate exception from the destructor.
                std::cerr << error.what() << std::endl;
            }
        }

        if (!this->context_.Get())
        {
            return;
        }

        if (this->context_.Get()->pb)
        {
            /* Close the output file. */
            avio_closep(&this->context_.Get()->pb);
        }
    }

    operator AVFormatContext * () const
    {
        return this->context_.Get();
    }

    AVFormatContext * operator->() const
    {
        return this->context_.Get();
    }

    /**
     ** This must be called after all outputs have been created with this
     ** context.
     **/
    void Initialize(Dictionary &codecOptions)
    {
        int result = avformat_write_header(
            this->context_.Get(),
            codecOptions.Get());

        if (result < 0)
        {
            throw ClipError(
                std::string("Error initializing OutputContext: ")
                + clip::AvErrorToString(result));
        }

        this->isInitialized_ = true;
    }

    /**
     ** Call this only when done writing to all associated outputs.
     **/
    void Finalize()
    {
        int result = av_write_trailer(this->context_.Get());

        if (result < 0)
        {
            throw ClipError(
                std::string("Error finalizing OutputContext: ")
                + clip::AvErrorToString(result));
        }

        this->isFinalized_ = true;
    }

    bool GetIsInitialized() const
    {
        return this->isInitialized_;
    }

    bool GetIsFinalized() const
    {
        return this->isFinalized_;
    }

protected:
    Context context_;
    bool isInitialized_;
    bool isFinalized_;
};



} // namespace clip
