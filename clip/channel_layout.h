/**
  * @file channel_layout.h
  *
  * @brief Wrapper around uint64_t, used by FFMPEG to represent channel layout.
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

#include <libavutil/common.h>
#include <libavutil/channel_layout.h>

}
FFMPEG_SHIM_POP_IGNORES


#include <cassert>
#include <string>
#include "clip/error.h"


namespace clip
{


namespace detail
{


class ChannelLayout
{
public:
    ChannelLayout(uint64_t mask)
        :
        layout_{}
    {
        int result = av_channel_layout_from_mask(&this->layout_, mask);

        if (result != 0)
        {
            throw ClipError(
                DescribeError("Failed to initialize channel layout", result));
        }
    }

    ~ChannelLayout()
    {
        av_channel_layout_uninit(&this->layout_);
    }

    ChannelLayout(const ChannelLayout &other)
        :
        layout_{}
    {
        int result = av_channel_layout_copy(&this->layout_, &other.layout_);

        if (result != 0)
        {
            throw ClipError(
                DescribeError("Failed to copy channel layout", result));
        }
    }

    ChannelLayout & operator=(const ChannelLayout &other)
    {
        // ffmpeg documentation claims that the destination will be
        // uninitialized prior to copy.
        int result = av_channel_layout_copy(&this->layout_, &other.layout_);

        if (result != 0)
        {
            throw ClipError(
                DescribeError("Failed to copy channel layout", result));
        }

        return *this;
    }

    ChannelLayout(ChannelLayout &&) = delete;
    ChannelLayout & operator=(ChannelLayout &&) = delete;

    int GetChannelCount() const
    {
        return this->layout_.nb_channels;
    }

    enum AVChannelOrder GetOrder() const
    {
        return this->layout_.order;
    }

    std::string Describe() const
    {
        std::string description;
        static constexpr size_t bufferSize = 64;
        description.resize(bufferSize);

        int result =
            av_channel_layout_describe(
                &this->layout_,
                description.data(),
                bufferSize);

        if (result < 0)
        {
            throw ClipError(
                DescribeError("Failed to describe layout", result));
        }

        if (static_cast<size_t>(result) > bufferSize)
        {
            description.resize(static_cast<size_t>(result));

            result =
                av_channel_layout_describe(
                    &this->layout_,
                    description.data(),
                    description.size());

            if (result < 0)
            {
                throw ClipError(
                    DescribeError("Failed to describe layout", result));
            }

            assert(static_cast<size_t>(result) == description.size());
        }
        else
        {
            description.resize(static_cast<size_t>(result));
        }

        return description;
    }

    void MakeCopy(AVChannelLayout *target) const
    {
        int result = av_channel_layout_copy(target, &this->layout_);

        if (result != 0)
        {
            throw ClipError(
                DescribeError("Failed to copy channel layout", result));
        }
    }

private:
    AVChannelLayout layout_;
};


} // end namespace detail


class ChannelLayout
{
public:
    ChannelLayout()
        :
        mask_(1),
        channelCount_(1),
        layoutName_()
    {

    }

    ChannelLayout(uint64_t channelLayout)
        :
        mask_(channelLayout),
        channelCount_(),
        layoutName_()
    {
        detail::ChannelLayout layout(this->mask_);
        this->channelCount_ = layout.GetChannelCount();
        this->layoutName_ = layout.Describe();
    }

    uint64_t Get() const
    {
        return this->mask_;
    }

    int GetChannelCount() const
    {
        return this->channelCount_;
    }

    std::string GetName() const
    {
        return this->layoutName_;
    }

    void MakeCopy(AVChannelLayout *target) const
    {
        detail::ChannelLayout layout(this->mask_);
        layout.MakeCopy(target);
    }

private:
    uint64_t mask_;
    int channelCount_;
    std::string layoutName_;
};


} // end namespace clip
