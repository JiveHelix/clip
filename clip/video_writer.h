/**
  * @file video_writer.h
  *
  * @brief Write video with stride.
  *
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/error.h"
#include "clip/video_output.h"
#include "tau/color_map.h"


namespace clip
{


size_t GetFieldSize(
    size_t height_pixels,
    const VideoOutput &output)
{
    return height_pixels * output.GetStride();
}


template<typename ColorMap>
static size_t GetDataWidth(size_t width_pixels)
{
    static constexpr auto pixelColorCount =
        tau::MatrixTraits<typename ColorMap::Colors>::columns;

    static_assert(pixelColorCount != Eigen::Dynamic);

    return width_pixels * static_cast<size_t>(pixelColorCount);
}


struct VideoWriter
{
public:
    VideoWriter(VideoOutput &output)
        :
        output_(output)
    {

    }

    template<typename Derived>
    void operator()(const Eigen::DenseBase<Derived> &data)
    {
        auto size = data.derived().size();

        assert(size > 0);

        // Copy bytes to avFrame
        AVFrame *avFrame = this->output_.GetNextFrame();

        memcpy(
            avFrame->data[0],
            data.derived().data(),
            static_cast<size_t>(size));

        this->output_.WriteFrame();
    }

    TimeStamp GetTimeStamp() const
    {
        return this->output_.GetTimeStamp();
    }

    void Flush()
    {
        this->output_.Flush();
    }

private:
    VideoOutput &output_;
};


struct StrideVideoWriter
{
public:
    using VideoFrame =
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    using StrideMap = Eigen::Map<VideoFrame, 0, Eigen::OuterStride<>>;

    StrideVideoWriter(
            size_t height_pixels,
            size_t dataWidth,
            VideoOutput &output)
        :
        output_(output),
        height_(static_cast<Eigen::Index>(height_pixels)),
        dataWidth_(static_cast<Eigen::Index>(dataWidth)),
        stride_(static_cast<Eigen::Index>(output.GetStride())),
        fieldSize_(GetFieldSize(height_pixels, output)),

        // Create a frame with the stride expected by the encoder.
        withStride_(this->height_, this->stride_),

        // Create a map with the expected stride.
        // This allows assignment to withStride through strideMap.
        // height x dataWidth data will be copied to withStride, and stride
        // count bytes will be left between each row of data.
        strideMap_(
            this->withStride_.data(),
            this->height_,
            this->dataWidth_,
            Eigen::OuterStride<>(this->stride_))
    {
        if (this->stride_ < this->dataWidth_)
        {
            throw VideoError("stride must be larger than dataWidth");
        }

    }

    template<typename Derived>
    void operator()(const Eigen::DenseBase<Derived> &data)
    {
        // strideMap maps onto withStride.
        // Assigning to it moves the values into place to match the encoder.
        this->strideMap_ =
            Eigen::Reshaped<
                const Derived,
                Eigen::Dynamic,
                Eigen::Dynamic,
                Eigen::RowMajor>(
                    data.derived(),
                    this->height_,
                    this->dataWidth_);

        // Copy bytes to avFrame
        AVFrame *avFrame = this->output_.GetNextFrame();
        memcpy(avFrame->data[0], this->withStride_.data(), this->fieldSize_);
        this->output_.WriteFrame();
    }

    TimeStamp GetTimeStamp() const
    {
        return this->output_.GetTimeStamp();
    }

    void Flush()
    {
        this->output_.Flush();
    }

private:
    VideoOutput &output_;
    Eigen::Index height_;
    Eigen::Index dataWidth_;
    Eigen::Index stride_;
    size_t fieldSize_;
    VideoFrame withStride_;
    StrideMap strideMap_;
};


template<typename Writer, typename ColorMap>
struct ColorMappedVideoWriter
{
public:
    ColorMappedVideoWriter(Writer &&writer, const ColorMap &colorMap)
        :
        writer_(std::forward<std::remove_cvref_t<Writer>>(writer)),
        colorMap_(colorMap),
        mapped_()
    {

    }

    template<typename Derived>
    void operator()(const Eigen::DenseBase<Derived> &data)
    {
        this->colorMap_(data, &this->mapped_);
        this->writer_(this->mapped_);
    }

private:
    std::remove_cvref_t<Writer> writer_;
    ColorMap colorMap_;
    typename ColorMap::Colors mapped_;
};


} // end namespace clip
