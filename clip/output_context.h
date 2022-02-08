#pragma once

extern "C"
{

#include <libavformat/avformat.h>

}

#include <string>
#include <iostream>
#include "clip/error.h"
#include "clip/dictionary.h"


namespace clip
{


class OutputContext
{
private:
    class Context
    { 
    public:
        Context(const std::string &fileName)
            :
            context_(NULL)
        {
            /* allocate the output media context */
            avformat_alloc_output_context2(
                &this->context_,
                NULL,
                NULL,
                fileName.c_str());

            if (!this->context_)
            {
                std::cerr << "Could not deduce output format from file extension: "
                    << "using MPEG." << std::endl;

                avformat_alloc_output_context2(
                    &this->context_,
                    NULL,
                    "mpeg",
                    fileName.c_str());
            }

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

        Context & operator=(Context &&other)
        {
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
    OutputContext(const std::string &fileName)
        :
        context_(fileName)
    {
        /* open the output file, if needed */
        if (!(this->context_.Get()->oformat->flags & AVFMT_NOFILE))
        {
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

    bool WriteHeader(Dictionary &codecOptions)
    {
        int result = avformat_write_header(this->context_.Get(), codecOptions.Get());

        if (result < 0)
        {
            std::cerr << "Error occurred writing header: "
                      << clip::AvErrorToString(result) << std::endl;

            return false;
        }
        
        return true;
    }

    bool WriteTrailer()
    {
        int result = av_write_trailer(this->context_.Get());

        if (result < 0)
        {
            std::cerr << "Error occurred writing trailer: "
                      << clip::AvErrorToString(result) << std::endl;

            return false;
        }
        
        return true;
    }

protected:
    Context context_;
};



} // namespace clip
