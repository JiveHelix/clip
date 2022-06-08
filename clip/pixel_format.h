/**
  * @file pixel_format.h
  * 
  * @brief Compile-time pixel formats.
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

#include <libavutil/pixfmt.h>

}
FFMPEG_SHIM_POP_IGNORES


#include <type_traits>
#include "clip/sample_format.h"


/*
 * Supported pixel formats
 *
 *
    AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    AV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...

    AV_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    AV_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    AV_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    AV_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    AV_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    AV_PIX_FMT_RGBA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_RGBA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    AV_PIX_FMT_BGRA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_BGRA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

*/


namespace clip
{


template<AVPixelFormat pixelFormat, typename Enable = void>
struct ColorType {};

template<AVPixelFormat pixelFormat>
struct ColorType
    <
        pixelFormat,
        std::enable_if_t<
            IsAnyOf(
                pixelFormat,
                AV_PIX_FMT_RGB24,
                AV_PIX_FMT_BGR24,
                AV_PIX_FMT_ARGB,
                AV_PIX_FMT_RGBA,
                AV_PIX_FMT_ABGR,
                AV_PIX_FMT_BGRA)
        >
    >
{
    using type = uint8_t;
};


template<AVPixelFormat pixelFormat>
struct ColorType
    <
        pixelFormat,
        std::enable_if_t<
            IsAnyOf(
                pixelFormat,
                AV_PIX_FMT_BGR48BE, 
                AV_PIX_FMT_BGR48LE, 
                AV_PIX_FMT_RGBA64BE,
                AV_PIX_FMT_RGBA64LE,
                AV_PIX_FMT_BGRA64BE,
                AV_PIX_FMT_BGRA64LE)
        >
    >
{
    using type = uint16_t;
};


template<AVPixelFormat pixelFormat, typename Enable = void>
struct ColorCount {};

template<AVPixelFormat pixelFormat>
struct ColorCount
    <
        pixelFormat,
        std::enable_if_t<
            IsAnyOf(
                pixelFormat,
                AV_PIX_FMT_RGB24,
                AV_PIX_FMT_BGR24,
                AV_PIX_FMT_BGR48BE, 
                AV_PIX_FMT_BGR48LE)
        >
    >
{
    static constexpr size_t value = 3;
};


template<AVPixelFormat pixelFormat>
struct ColorCount
    <
        pixelFormat,
        std::enable_if_t<
            IsAnyOf(
                pixelFormat,
                AV_PIX_FMT_ARGB,
                AV_PIX_FMT_RGBA,
                AV_PIX_FMT_ABGR,
                AV_PIX_FMT_BGRA,
                AV_PIX_FMT_RGBA64BE,
                AV_PIX_FMT_RGBA64LE,
                AV_PIX_FMT_BGRA64BE,
                AV_PIX_FMT_BGRA64LE)
        >
    >
{
    static constexpr size_t value = 4;
};


template<AVPixelFormat pixelFormat>
struct PixelTraits
{
    using Color = typename ColorType<pixelFormat>::type;
    static constexpr auto colorCount = ColorCount<pixelFormat>::value;
    static constexpr auto sizeBytes = sizeof(Color) * colorCount;
};


// We do not always know the pixel format at compile-time.
// Look of pixel sizes with a runtime AVPixelFormat.
struct PixelSize
{
    size_t colorCount;
    size_t sizeBytes;
};


PixelSize GetPixelSize(AVPixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
        case AV_PIX_FMT_RGB24:
        {
            using Traits = PixelTraits<AV_PIX_FMT_RGB24>;
            return {Traits::colorCount, Traits::sizeBytes};
        }
            
        case AV_PIX_FMT_BGR24:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGR24>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_ARGB:
        {
            using Traits = PixelTraits<AV_PIX_FMT_ARGB>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_RGBA:
        {
            using Traits = PixelTraits<AV_PIX_FMT_RGBA>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_ABGR:
        {
            using Traits = PixelTraits<AV_PIX_FMT_ABGR>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_BGRA:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGRA>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_BGR48BE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGR48BE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_BGR48LE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGR48LE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_RGBA64BE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_RGBA64BE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_RGBA64LE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_RGBA64LE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_BGRA64BE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGRA64BE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        case AV_PIX_FMT_BGRA64LE:
        {
            using Traits = PixelTraits<AV_PIX_FMT_BGRA64LE>;
            return {Traits::colorCount, Traits::sizeBytes};
        }

        default:
            // Note:
            //
            // Other formats are supported by FFMPEG, we just don't provide
            // traits for them in this wrapper.
            // Please add any others you need.
            throw ClipError("Unsupported pixel format");
    }
}


} // end namespace clip
