/**
  * @file color_mapped_writer_detail.h
  * 
  * @brief Implements writing color-mapped data with and without stride.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/video_writer.h"
#include "tau/eigen.h"


namespace clip
{


namespace detail
{


template<typename Reader, typename ColorMap>
void WriteColorMappedWithStride(
    Reader &reader,
    const ColorMap &colorMap,
    VideoOutput &output)
{
    static_assert(
        tau::MatrixTraits<typename Reader::Matrix>::isRowMajor,
        "Expected row major data to match AVFrame.");

    ColorMappedVideoWriter strideFrameWriter(
        StrideVideoWriter(
            reader.GetHeight_pixels(),
            GetDataWidth<ColorMap>(reader.GetWidth_pixels()),
            output),
        colorMap);

    while (reader.HasFrame())
    {
        strideFrameWriter(reader.GetNextFrameData());
    }
}


template<typename Reader, typename ColorMap>
void WriteColorMapped(
    Reader &reader,
    const ColorMap &colorMap,
    VideoOutput &output)
{
    static_assert(
        tau::MatrixTraits<typename Reader::Matrix>::isRowMajor,
        "Expected row major data to match AVFrame.");

    ColorMappedVideoWriter frameWriter(VideoWriter(output), colorMap);

    while (reader.HasFrame())
    {
        frameWriter(reader.GetNextFrameData());
    }
}


} // end namespace detail


} // end namespace clip
