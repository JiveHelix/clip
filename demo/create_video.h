/**
  * @file create_video.h
  *
  * @brief Create a generated video.
  *
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/time_stamp.h"
#include "clip/pixel_format.h"
#include "clip/format.h"
#include "clip/output_context.h"
#include "clip/video_output.h"
#include "clip/video_writer.h"


template<typename VideoWriter, typename VideoGenerator>
void GenerateVideo(
    VideoWriter &&videoWriter,
    VideoGenerator &&videoGenerator,
    clip::TimeStamp duration)
{
    typename std::remove_cvref_t<VideoGenerator>::Output videoOutput;

    while (videoWriter.GetTimeStamp() <= duration)
    {
        videoGenerator.FillFrame(&videoOutput);
        videoWriter(videoOutput);
    }

    videoWriter.Flush();
}


template<typename Format, typename Generator>
void CreateVideo(
    const std::string &outputBaseName,
    clip::VideoOptions videoOptions,
    clip::TimeStamp duration)
{
    clip::Dictionary codecOptions;

    const auto *outputFormat = Format::Get();

    auto outputContext = std::make_shared<clip::OutputContext>(
        outputFormat,
        outputBaseName + "." + Format::extension);

    assert(videoOptions.inPixelFormat == AV_PIX_FMT_RGB24);

    clip::VideoOutput videoOutput(outputContext, codecOptions, videoOptions);

    outputContext->Initialize(codecOptions);

    static constexpr auto pixelSizeBytes = Generator::ColorMap::pixelSizeBytes;

    assert(
        pixelSizeBytes
        == GetPixelSize(videoOptions.inPixelFormat).sizeBytes);

    auto dataWidth = static_cast<size_t>(videoOptions.width) * pixelSizeBytes;

    if (dataWidth == videoOutput.GetStride())
    {
        std::cout << "Frame width matches stride." << std::endl;

        GenerateVideo(
            clip::VideoWriter(videoOutput),
            Generator(
                videoOptions.height,
                videoOptions.width,
                videoOptions.framesPerSecond),
            duration);
    }
    else
    {
        std::cout << "Frame width does not match stride." << std::endl;

        auto videoWriter = clip::StrideVideoWriter(
            static_cast<size_t>(videoOptions.height),
            dataWidth,
            videoOutput);

        GenerateVideo(
            clip::StrideVideoWriter(
                static_cast<size_t>(videoOptions.height),
                dataWidth,
                videoOutput),
            Generator(
                videoOptions.height,
                videoOptions.width,
                videoOptions.framesPerSecond),
            duration);
    }

    outputContext->Finalize();
}
