/**
  * @file time_stamp.h
  * 
  * @brief Time stamp wrapper combining count and AVRational time base.
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

#include <libavutil/avutil.h>
#include <libavutil/rational.h>

}
FFMPEG_SHIM_POP_IGNORES


#include <ostream>
#include <fmt/core.h>
#include "clip/error.h"


namespace clip
{


CREATE_EXCEPTION(TimeStampError, VideoError);


class TimeStamp
{
public:
    TimeStamp()
        :
        count_(0),
        timeBase_(AVRational{1, 1})
    {

    }

    explicit TimeStamp(
        int64_t count,
        AVRational timeBase = AVRational{1, 1})
        :
        count_(count),
        timeBase_(timeBase)
    {
        if (timeBase.num == 0 || timeBase.den == 0)
        {
            throw TimeStampError("Invalid time base.");
        }
    }

    int64_t Count() const
    {
        return this->count_; 
    }

    const AVRational & GetTimeBase() const
    {
        return this->timeBase_;
    }

    TimeStamp & operator++()
    {
        this->count_ += 1;
        return *this;
    }

    TimeStamp operator++(int)
    {
        TimeStamp old = *this;
        this->operator++();
        return old;
    }

    TimeStamp & operator--()
    {
        this->count_ -= 1;
        return *this;
    }

    TimeStamp operator--(int)
    {
        TimeStamp old = *this;
        this->operator--();
        return old;
    }

    TimeStamp & operator+=(int64_t count)
    {
        this->count_ += count; 
        return *this;
    }

    TimeStamp & operator-=(int64_t count)
    {
        this->count_ -= count; 
        return *this;
    }

    TimeStamp & operator=(int64_t count)
    {
        this->count_ = count; 
        return *this;
    }

    bool operator<(const TimeStamp &other)
    {
        int compare = av_compare_ts(
            this->count_,
            this->timeBase_,
            other.count_,
            other.timeBase_);

        return (compare < 0);
    }

    bool operator>(const TimeStamp &other)
    {
        int compare = av_compare_ts(
            this->count_,
            this->timeBase_,
            other.count_,
            other.timeBase_);

        return (compare > 0);
    }

    bool operator==(const TimeStamp &other)
    {
        int compare = av_compare_ts(
            this->count_,
            this->timeBase_,
            other.count_,
            other.timeBase_);

        return (compare == 0);
    }

    bool operator!=(const TimeStamp &other)
    {
        return !(*this == other);
    }

    bool operator>=(const TimeStamp &other)
    {
        return !(*this < other);
    }

    bool operator<=(const TimeStamp &other)
    {
        return !(*this > other);
    }

    std::ostream & ShowIntegral(std::ostream &outputStream) const
    {
        if (this->count_ == AV_NOPTS_VALUE)
        {
            return outputStream << "NOPTS";
        }
        else
        {
            return outputStream << this->count_;
        }
    }

    std::ostream & ShowSeconds(std::ostream &outputStream) const
    {
        return outputStream
            << fmt::format("{:.6g}", av_q2d(this->timeBase_) * this->count_);
    }

private:
    int64_t count_;
    AVRational timeBase_;
};


std::ostream &operator<<(std::ostream &outputStream, const TimeStamp &timeStamp)
{
    return timeStamp.ShowIntegral(outputStream);
}


class Seconds
{
public:
    Seconds(const TimeStamp &timeStamp)
        :
        timeStamp(timeStamp)
    {

    }

    const TimeStamp &timeStamp;
};


std::ostream &operator<<(std::ostream &outputStream, const Seconds &seconds)
{
    return seconds.timeStamp.ShowSeconds(outputStream);
}


} // end namespace clip
