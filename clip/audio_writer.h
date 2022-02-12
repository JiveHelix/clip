/**
  * @file audio_writer.h
  * 
  * @brief Implements writing planar and interleaved audio samples.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/audio_output.h"


namespace clip
{


template<typename Options>
struct MonoAudioWriter
{
public:
    MonoAudioWriter(clip::AudioOutput<Options> &audioOutput)
        :
        audioOutput_(audioOutput)
    {

    }

    template<typename T>
    void operator()(T &data)
    {
        using Sample = typename Options::Format::type;

        int channelCount =
            this->audioOutput_.GetOptions().channelLayout.GetChannelCount();

        AVFrame *frame = this->audioOutput_.GetNextFrame();

        assert(
            static_cast<size_t>(frame->nb_samples)
            == this->audioOutput_.GetSampleCount());

        assert(
            static_cast<size_t>(data.size())
            == this->audioOutput_.GetSampleCount());

        if constexpr (Options::Format::isPlanar)
        {
            size_t fieldSize =
                sizeof(Sample) * this->audioOutput_.GetSampleCount();

            assert(frame->linesize[0] == static_cast<int>(fieldSize));

            // Each channel is stored in its own data plane.
            // Copy the same signal to each channel.
            for (int i = 0; i < channelCount; ++i)
            {
                assert(frame->data[i] != NULL);

                assert(
                    frame->nb_samples
                    == static_cast<int>(this->audioOutput_.GetSampleCount()));

                memcpy(
                    frame->data[i],
                    data.data(),
                    fieldSize);

                assert(
                    frame->nb_samples
                    == static_cast<int>(this->audioOutput_.GetSampleCount()));
            }
        }
        else
        {
            assert(
                frame->linesize[0]
                == static_cast<int>(
                    this->audioOutput_.GetSampleCount() * sizeof(Sample)));

            Sample *out = reinterpret_cast<Sample *>(frame->data[0]);

            // The channels are interleaved.
            // Copy each value channelCount times
            for (int j = 0; j < frame->nb_samples; j++)
            {
                for (int i = 0; i < channelCount; i++)
                {
                    *out++ = data(j);
                }
            }
        }

        this->audioOutput_.WriteFrame();
    }

    TimeStamp GetTimeStamp() const
    {
        return this->audioOutput_.GetTimeStamp();
    }

    void Flush()
    {
        this->audioOutput_.Flush();
    }

private:
    clip::AudioOutput<Options> &audioOutput_;
};


} // end namespace clip
