
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ostream>
#include <iostream>

#include "clip/audio_output.h"
#include "clip/time_stamp.h"
#include "clip/output_context.h"
#include "clip/video_output.h"
#include "clip/audio_sweep.h"
#include "clip/rgb_test_pattern.h"


static const clip::TimeStamp streamDuration(15);


template<typename Output, typename Generator>
static void GenerateFrame(
    Output &&output,
    Generator &&generator)
{
    generator.FillFrame(output.GetNextFrame());
    output.WriteFrame();
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        return EXIT_FAILURE;
    }

    std::string fileName(argv[1]);

    clip::Dictionary codecOptions;

    clip::OutputContext outputContext(fileName);

    std::unique_ptr<clip::VideoOutput> videoOutput;

    using Options = clip::AudioOptions<AV_SAMPLE_FMT_S16P>;

    Options audioOptions{
        44100,
        128000,
        AV_CH_LAYOUT_STEREO};

    std::unique_ptr<clip::AudioOutput<Options>> audioOutput;
    std::unique_ptr<clip::AudioSweep<Options>> audioSweep;
    std::unique_ptr<clip::RgbTestPattern> rgbTestPattern;

    const AVOutputFormat *format = outputContext->oformat;

    if (format->video_codec == AV_CODEC_ID_NONE)
    {
        std::cerr
            << "This demo is intended to demonstrate video. "
            << "No video codec found." << std::endl;

        return EXIT_FAILURE;
    }

#ifdef LOSSLESS
    auto videoOptions = clip::VideoOptions::MakeLossless();
#else
    auto videoOptions = clip::VideoOptions::MakeDefault();
#endif

    videoOutput = std::make_unique<clip::VideoOutput>(
        outputContext,
        codecOptions,
        videoOptions);

    rgbTestPattern =
        std::make_unique<clip::RgbTestPattern>(videoOptions);

    bool encodeAudio = false;

    if (format->audio_codec != AV_CODEC_ID_NONE)
    {
        audioOutput = std::make_unique<clip::AudioOutput<Options>>(
            outputContext,
            codecOptions,
            audioOptions);

        encodeAudio = true;

        audioSweep = std::make_unique<clip::AudioSweep<Options>>(
            audioOptions,
            20.0,
            40.0,
            400.0);
    }

    if (!outputContext.WriteHeader(codecOptions))
    {
        std::cerr << "Failed to write header." << std::endl;
        return EXIT_FAILURE;
    }

    if (encodeAudio)
    {
        while (
            (videoOutput->GetTimeStamp() <= streamDuration)
            && (audioOutput->GetTimeStamp() <= streamDuration))
        {
            while (audioOutput->GetTimeStamp() < videoOutput->GetTimeStamp())
            {
                // Send audio frames until we catch up.
                GenerateFrame(*audioOutput, *audioSweep);
            }

            GenerateFrame(*videoOutput, *rgbTestPattern);
        }

        videoOutput->Flush();
        audioOutput->Flush();
    }
    else
    {
        while (videoOutput->GetTimeStamp() <= streamDuration)
        {
            GenerateFrame(*videoOutput, *rgbTestPattern);
        }

        videoOutput->Flush();
    }
    
    if (!outputContext.WriteTrailer())
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
