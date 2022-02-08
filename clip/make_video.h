#pragma once

#include <limits>
#include <Eigen/Dense>
#include <cassert>
#include "clip/output_context.h"
#include "clip/video_output.h"


template<typename T, bool isRowMajor_, typename Source>
class Data
{
public:
    using type = T;

    static constexpr bool isRowMajor = isRowMajor_;

    static constexpr int order = isRowMajor ? Eigen::RowMajor : Eigen::ColMajor;

    using Size = Eigen::Index;

    static constexpr auto EigenMaximum =
        static_cast<size_t>(std::numeric_limits<Size>::max());
    
    Data(size_t height, size_t width, const Source &source)
        :
        height_(static_cast<Size>(height)),
        width_(static_cast<Size>(width)),
        source_(source)
    {
        if (height > EigenMaximum)
        {
            throw VideoError("height exceeds maximum");
        }

        if (width > EigenMaximum)
        {
            throw VideoError("width exceeds maximum");
        }

        if (width * height > EigenMaximum)
        {
            throw VideoError("frame size exceeds maximum");
        }
    }

    Size GetHeight() const
    {
        return this->height_;
    }

    Size GetWidth() const
    {
        return this->width_;
    }

    Size GetFramePixelCount() const
    {
        return this->height_ * this->width_;
    }
    
    const type * GetNextFrame()
    {
        return this->source_.GetNextFrame();
    }

    void Seek(ssize_t frameIndex)
    {
        this->source_.Seek(frameIndex);
    }

protected:
    Size height_;
    Size width_;
    Source source_;
};


template<typename Data>
using MapType = Eigen::Map<
    Eigen::Matrix<
        typename Data::type,
        Eigen::Dynamic,
        Eigen::Dynamic,
        Data::order>>;


/**

Supported input formats

AV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
AV_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...


**/

template<template Data, template ColorMap>
void MakeColorVideo(
    const std::string &fileName,
    Data &data,
    const ColorMap &colorMap,
    clip::VideoOptions options,
    clip::Dictionary *codecOptions = NULL)
{
    assert(options.width * options.height == data.GetFramePixelCount());

    clip::OutputContext outputContext(fileName);
    outputContext.Open();
    
    clip::Dictionary codecOptions_;

    if (codecOptions)
    {
        codecOptions_ = *codecOptions;
    }

    auto videoOutput = clip::VideoOutput(
        outputContext,
        codecOptions_,
        videoOptions);

    const Data::type *dataFrame = data.GetNextFrame();
    
    while (dataFrame)
    {
        MapType<Data> asMatrix(dataFrame, data.GetHeight(), data.GetWidth());
        typename ColorMap::OutputType colored;
        colorMap(asMatrix, &colored);

        AVFrame *frame = videoOutput.GetNextFrame();


        dataFrame = data.GetNextFrame();
    }
}
