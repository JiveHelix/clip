/**
  * @file audio_and_video.cpp
  * 
  * @brief Demonstrates the generation of a video with an audio sweep.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ostream>
#include <iostream>

#include "clip/audio_output.h"
#include "clip/time_stamp.h"
#include "clip/output_context.h"
#include "clip/video_output.h"
#include "clip/video_writer.h"
#include "clip/audio_writer.h"
#include "clip/audio_sweep.h"
#include "clip/circle_gradient.h"
#include "clip/pixel_format.h"
#include "clip/format.h"


static const clip::TimeStamp streamDuration(15);


template<
    typename VideoWriter,
    typename VideoGenerator,
    typename AudioWriter,
    typename AudioGenerator>
void GenerateAudioAndVideo(
    VideoWriter &&videoWriter,
    VideoGenerator &&videoGenerator,
    AudioWriter &&audioWriter,
    AudioGenerator &&audioGenerator,
    clip::TimeStamp duration)
{
    typename std::remove_cvref_t<VideoGenerator>::Output videoOutput;
    typename std::remove_cvref_t<AudioGenerator>::Output audioOutput;

    while (
        (videoWriter.GetTimeStamp() <= duration)
        && (audioWriter.GetTimeStamp() <= duration))
    {
        while (audioWriter.GetTimeStamp() < videoWriter.GetTimeStamp())
        {
            // Send audio frames until we catch up.
            audioGenerator.FillFrame(&audioOutput);
            audioWriter(audioOutput);
        }

        videoGenerator.FillFrame(&videoOutput);
        videoWriter(videoOutput);
    }

    videoWriter.Flush();
    audioWriter.Flush();
}


template<typename Format, typename Generator, typename AudioOptions>
void CreateAudioAndVideo(
    const std::string &baseName,
    clip::VideoOptions videoOptions,
    AudioOptions &&audioOptions,
    clip::TimeStamp duration)
{
    clip::Dictionary codecOptions;

    const AVOutputFormat *outputFormat;

    outputFormat = Format::Get();

    auto outputContext = std::make_shared<clip::OutputContext>(
        outputFormat,
        baseName + "." + Format::extension);

    assert(videoOptions.inPixelFormat == AV_PIX_FMT_RGB24);

    static constexpr auto pixelSizeBytes = Generator::ColorMap::pixelSizeBytes;

    assert(
        pixelSizeBytes
        == GetPixelSize(videoOptions.inPixelFormat).sizeBytes);

    using Options = std::remove_cvref_t<AudioOptions>;

    clip::VideoOutput videoOutput(outputContext, codecOptions, videoOptions);

    clip::AudioOutput<Options> audioOutput(
        outputContext,
        codecOptions,
        audioOptions);

    outputContext->Initialize(codecOptions);

    Generator generator(
        videoOptions.height,
        videoOptions.width,
        videoOptions.framesPerSecond);

    auto dataWidth = static_cast<size_t>(videoOptions.width) * pixelSizeBytes;

    clip::AudioSweep<Options> audioSweep(
        audioOptions,
        20.0,
        40.0,
        400.0,
        audioOutput.GetSampleCount());

    if (dataWidth == videoOutput.GetStride())
    {
        std::cout << "Frame width matches stride." << std::endl;

        GenerateAudioAndVideo(
            clip::VideoWriter(videoOutput),
            generator,
            clip::MonoAudioWriter(audioOutput),
            audioSweep,
            duration);
    }
    else
    {
        std::cout << "Frame width does not match stride." << std::endl;

        GenerateAudioAndVideo(
            clip::StrideVideoWriter(
                static_cast<size_t>(videoOptions.height),
                dataWidth,
                videoOutput),
            generator,
            clip::MonoAudioWriter(audioOutput),
            audioSweep,
            duration);
    }

    outputContext->Finalize();
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " baseName" << std::endl;
        return EXIT_FAILURE;
    }

    std::string baseName(argv[1]);

    try
    {
        CreateAudioAndVideo
        <
            clip::format::Mp4,
            clip::CircleGradientColors<uint16_t>
        >(
            baseName,
            clip::VideoOptions::MakeDefault(),
            clip::AudioOptions<AV_SAMPLE_FMT_S16P>{
                44100,
                128000,
                AV_CH_LAYOUT_STEREO},
            streamDuration);
    }
    catch (clip::ClipError &error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
