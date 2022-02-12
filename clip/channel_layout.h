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


extern "C"
{

#include <libavutil/common.h>
#include <libavutil/channel_layout.h>

}

#include <string>


namespace clip
{


class ChannelLayout
{
public:
    ChannelLayout()
        :
        channelLayout_(1),
        channelCount_(1)
    {

    }

    ChannelLayout(uint64_t channelLayout)
        :
        channelLayout_(channelLayout),
        channelCount_(
            av_get_channel_layout_nb_channels(channelLayout)),
        layoutName_()
    {
        // The interface of libavutil/channel_layout.h forces us to iterate
        // over all indices of standard layouts to find the name.
        int result = 0;
        unsigned index = 0;

        uint64_t layout;
        const char *name = NULL;

        while (true)
        {
            result = av_get_standard_channel_layout(
                index++,
                &layout,
                &name);    

            if (result == AVERROR_EOF)
            {
                this->layoutName_ = "Non-standard Channel Layout";
                break;
            }

            if (layout == this->channelLayout_)
            {
                // This is the layout we were looking for.
                this->layoutName_ = name;
                break;
            }
        }
    }

    uint64_t Get() const
    {
        return this->channelLayout_; 
    }

    int GetChannelCount() const
    {
        return this->channelCount_;
    }

    std::string GetName() const
    {
        return this->layoutName_;
    }

private:
    uint64_t channelLayout_;
    int channelCount_;
    std::string layoutName_;
};


} // end namespace clip
