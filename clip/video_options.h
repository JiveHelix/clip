/**
  * @file video_options.h
  *
  * @brief Options to configure clip::VideoOutput.
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

#include <libavcodec/avcodec.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/resolution.h"


namespace clip
{


enum class Preset: uint8_t
{
    superfast = 0,
    veryfast,
    faster,
    fast,
    medium,
    slow,
    slower,
    veryslow,
    placebo
};


const char * presetStrings[] = {
    "superfast",
    "veryfast",
    "faster",
    "fast",
    "medium",
    "slow",
    "slower",
    "veryslow",
    "placebo"};


struct VideoOptions
{
    int height;
    int width;
    AVPixelFormat inPixelFormat;
    AVPixelFormat outPixelFormat;
    int framesPerSecond;
    int qualityFactor;
    int64_t bitRate;
    int gopSize;
    int profile;
    int level;
    AVCodecID codecId;
    Preset preset;

    static VideoOptions MakeDefault(const Resolution &resolution)
    {
        VideoOptions result
        {
            .height = resolution.height,
            .width = resolution.width,
            .inPixelFormat = AV_PIX_FMT_RGB24,
            .outPixelFormat = AV_PIX_FMT_YUV420P,
            .framesPerSecond = 30,
            .qualityFactor = 22,
            .bitRate = -1,
            .gopSize = 32,
            .profile = FF_PROFILE_H264_HIGH,
            .level = 51,
            .codecId = AV_CODEC_ID_H264,
            .preset = Preset::medium
        };

        return result;
    }

    static VideoOptions MakeLossless(const Resolution &resolution)
    {
        auto result = MakeDefault(resolution);
        result.qualityFactor = 0;
        result.profile = FF_PROFILE_H264_HIGH_444;
        result.gopSize = 1;
        result.outPixelFormat = AV_PIX_FMT_YUV444P;
        result.preset = Preset::veryslow;

        return result;
    }
};


} // end namespace clip
