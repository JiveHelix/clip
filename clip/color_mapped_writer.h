/**
  * @file color_mapped_writer.h
  * 
  * @brief Writes color-mapped frames from `Reader`, any class that implements: 
  *     static type Matrix,
  *
  *     methods:
  *         size_t GetHeight_pixels()
  *         size_t GetWidth_pixels()
  *         bool HasFrame()
  *         Matrix GetNextFrameData()
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "detail/color_mapped_writer_detail.h"


namespace clip
{


template<typename Reader, typename ColorMap>
void WriteColorMapped(
    Reader &reader,
    const ColorMap &colorMap,
    VideoOutput &output)
{
    auto dataWidth = GetDataWidth<ColorMap>(reader.GetWidth_pixels());
    auto stride = output.GetStride();
    
    if (dataWidth == stride)
    {
        detail::WriteColorMapped(reader, colorMap, output);
    }
    else
    {
        detail::WriteColorMappedWithStride(reader, colorMap, output);
    }

    output.Flush();
}


}
