#pragma once

extern "C"
{

#include <libavutil/frame.h>

}

#include <cmath>
#include <cassert>


namespace clip
{


template<typename Options>
class AudioSweep
{
public:
    static constexpr double pi = static_cast<double>(M_PI);

    AudioSweep(
        const Options &options,
        double start_hz,
        double increase_hzPerSecond,
        double stop_hz)
        :
        options_(options),
        start_hz_(start_hz),
        stop_hz_(stop_hz),
        time_(0.0),
        samplePeriod_(2 * pi * start_hz / options.sampleRate),
        samplePeriodIncrement_(
            2 * pi * increase_hzPerSecond
            / (options.sampleRate * options.sampleRate)),
        maxSamplePeriod_(2 * pi * stop_hz / options.sampleRate)
    {

    }

    void Reset()
    {
        this->time_ = 0.0;
        this->samplePeriod_ = 
            2 * pi * this->start_hz_ / this->options_.sampleRate;
    }

    float GetStart_hz() const
    {
        return this->start_hz_;
    }

    float GetStop_hz() const
    {
        return this->stop_hz_;
    }

    static double GetSample(double time)
    {
        // Scale the integral value to a sensible range.
        double value1 = sin(time);

        double value2 = sin(2.0 * time) * .75;

        double value3 = sin(2.5 * time) * .8;
        
        return (value1 + value2 + value3) / 3.0;
    }


    void FillFrame(AVFrame *frame)
    {
        using Sample = typename Options::Format::type;
        int channelCount = this->options_.channelLayout.GetChannelCount();
        Sample *out = reinterpret_cast<Sample *>(frame->data[0]);

        assert(
            frame->linesize[0]
            >= frame->nb_samples * static_cast<int>(sizeof(Sample)));

        for (int j = 0; j < frame->nb_samples; j++)
        {
            Sample value;

            if constexpr (std::is_integral_v<Sample>)
            {
                // Scale the integral value to a sensible range.
                value = static_cast<Sample>(GetSample(this->time_) * 20000);
            }
            else
            {
                // Leave the floating-point value between -1.0 and 1.0
                value = static_cast<Sample>(GetSample(this->time_));
            }

            for (int i = 0; i < channelCount; i++)
            {
                if constexpr (Options::Format::isPlanar)
                {
                    // Each channel is stored in its own data plane.
                    out = reinterpret_cast<Sample *>(frame->data[i]) + j;
                    *out = value;
                }
                else
                {
                    // Interleaved.
                    *out++ = value;
                }
            }

            this->time_ += this->samplePeriod_;

            if (this->samplePeriod_ < this->maxSamplePeriod_)
            {
                this->samplePeriod_ += this->samplePeriodIncrement_;
            }

        }
    }

private:
    Options options_;
    double start_hz_;
    double stop_hz_;
    double time_;
    double samplePeriod_;
    double samplePeriodIncrement_;
    double maxSamplePeriod_;
};


} // namespace clip
