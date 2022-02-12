/**
  * @file audio_sweep.h
  * 
  * @brief Generate a swept audio signal.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


extern "C"
{

#include <libavutil/frame.h>

}

#include <cmath>
#include <cassert>
#include "tau/angles.h"
#include "Eigen/Dense"
#include "clip/sample_traits.h"


namespace clip
{


template<typename Options>
class AudioSweep
{
public:

    using Sample = typename Options::Format::type;
    
    using Output = Eigen::VectorX<Sample>;

    static constexpr auto pi = tau::Angles<double>::pi;

    AudioSweep(
        const Options &options,
        double start_hz,
        double increase_hzPerSecond,
        double stop_hz,
        size_t sampleCount)
        :
        options_(options),
        start_hz_(start_hz),
        stop_hz_(stop_hz),
        sampleCount_(static_cast<ssize_t>(sampleCount)),
        time_(0.0),
        samplePeriod_(2 * pi * start_hz / options.sampleRate),
        samplePeriodIncrement_(
            2 * pi * increase_hzPerSecond
            / (options.sampleRate * options.sampleRate)),
        maxSamplePeriod_(2 * pi * stop_hz / options.sampleRate)
    {
        assert(sampleCount <= std::numeric_limits<ssize_t>::max());
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
        // Harmonics:
        //  1st .5
        //  2nd 1
        //  3rd 1.5
        //  4th 2.0
        //  5th 2.5
        //
        // Build up three different tones at the 2nd, 4th, and 5th harmonics.
        double value1 = sin(time);
        double value2 = sin(2.0 * time) * .75;
        double value3 = sin(2.5 * time) * .8;
        
        return (value1 + value2 + value3) / 3.0;
    }

    void FillFrame(Output *output)
    {
        if (output->size() != this->sampleCount_)
        {
            output->resize(this->sampleCount_);
        }

        for (ssize_t i = 0; i < this->sampleCount_; ++i)
        {
            (*output)[i] = Scale<SampleTraits<Sample>>(GetSample(this->time_));

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
    ssize_t sampleCount_;
    double time_;
    double samplePeriod_;
    double samplePeriodIncrement_;
    double maxSamplePeriod_;
};


} // namespace clip
