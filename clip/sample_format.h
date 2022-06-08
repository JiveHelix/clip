/**
  * @file sample_format.h
  * 
  * @brief Traits of supported audio sample formats.
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

#include <libavutil/samplefmt.h>

}
FFMPEG_SHIM_POP_IGNORES


#include <type_traits>


namespace clip
{


template<AVSampleFormat sampleFormat>
struct SampleType {};

template<>
struct SampleType<AV_SAMPLE_FMT_U8>
{
    using type = uint8_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_U8P>
{
    using type = uint8_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S16>
{
    using type = int16_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S16P>
{
    using type = int16_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S32>
{
    using type = int32_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S32P>
{
    using type = int32_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S64>
{
    using type = int64_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_S64P>
{
    using type = int64_t;
};

template<>
struct SampleType<AV_SAMPLE_FMT_FLT>
{
    using type = float;
};

template<>
struct SampleType<AV_SAMPLE_FMT_FLTP>
{
    using type = float;
};

template<>
struct SampleType<AV_SAMPLE_FMT_DBL>
{
    using type = double;
};

template<>
struct SampleType<AV_SAMPLE_FMT_DBLP>
{
    using type = double;
};


template<typename First, typename... Others>
static constexpr bool IsAnyOf(First &&first, Others &&...others)
{
    return ((first == others) || ...);
}


template<AVSampleFormat sampleFormat, typename = std::void_t<>>
struct IsPlanar: std::false_type {};

template<AVSampleFormat sampleFormat>
struct IsPlanar<
    sampleFormat,
    std::void_t<
        std::enable_if_t<
            IsAnyOf(
                sampleFormat,
                AV_SAMPLE_FMT_U8P,
                AV_SAMPLE_FMT_S16P,
                AV_SAMPLE_FMT_S32P,
                AV_SAMPLE_FMT_S64P,
                AV_SAMPLE_FMT_FLTP,
                AV_SAMPLE_FMT_DBLP)
        >
    >
>: std::true_type
{

};


template<AVSampleFormat sampleFormat>
struct SampleFormat
{
    using type = typename SampleType<sampleFormat>::type;
    static constexpr bool isPlanar = IsPlanar<sampleFormat>::value;
    static constexpr AVSampleFormat value = sampleFormat;
};


} // end namespace clip
