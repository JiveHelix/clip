#pragma once


namespace clip
{


enum class Preset: uint8_t
{
    superfast = 0,
    verfast,
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
    "verfast",
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
    Preset preset;

    static VideoOptions MakeDefault()
    {
        VideoOptions result
        {
            .height = 1080,
            .width = 1920,
            .inPixelFormat = AV_PIX_FMT_RGB24,
            .outPixelFormat = AV_PIX_FMT_YUV420P,
            .framesPerSecond = 30,
            .qualityFactor = 22,
            .bitRate = -1,
            .gopSize = 32,
            .profile = FF_PROFILE_H264_HIGH,
            .preset = Preset::medium
        };

        return result;
    }

    static VideoOptions MakeLossless()
    {
        auto result = MakeDefault();
        result.qualityFactor = 0;
        result.profile = FF_PROFILE_H264_HIGH_444;
        result.gopSize = 1;
        result.outPixelFormat = AV_PIX_FMT_YUV444P;
        result.preset = Preset::veryslow;

        return result;
    }
};


} // end namespace clip
