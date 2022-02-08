#pragma once

extern "C"
{

#include <libavutil/frame.h>

}

#include <cmath>
#include "tau/angles.h"
#include "tau/color_map.h"
#include "tau/color_maps/turbo.h"
#include "clip/video_options.h"


namespace clip
{


class RgbTestPattern
{
public:
    RgbTestPattern(const VideoOptions &options)
        :
        time_(0.0),
        pathStep_(tau::Angles<double>::tau / (6 * options.framesPerSecond)),
        height_(options.height),
        width_(options.width),
        center_{options.width / 2, options.height / 2},
        radius_(std::min(options.height, options.width) / 3),
        raw_(options.height, options.width),
        colorMap_(
            tau::turbo::MakeRgb8(
                static_cast<size_t>(
                    GetDistance(
                        options.height / 2,
                        options.width / 2)
                    + this->radius_)))
    {

    }

    void Reset()
    {
        this->time_ = 0.0;
    }

    void FillFrame(AVFrame *avFrame)
    {
        this->ComputeRawDistances_(this->GetNextPath());

        int stride = avFrame->linesize[0];
        int expectedWidth = this->width_ * 3;
        int height = this->height_;

        ColorMap::OutputType mapped;
        this->colorMap_(this->raw_, &mapped);

        const uint8_t *data;
        size_t fieldSize;

        if (stride != expectedWidth)
        {
            assert(stride > expectedWidth);
            fieldSize = static_cast<size_t>(height * stride);

            VideoFrame withStride(height, stride);

            Map target(
                withStride.data(),
                height,
                expectedWidth,
                Eigen::OuterStride<>(stride));

            target = mapped.reshaped<Eigen::RowMajor>(
                height,
                expectedWidth);
            
            data = withStride.data();
        }
        else
        {
            fieldSize = static_cast<size_t>(height * expectedWidth);
            data = mapped.data();
        }

        memcpy(avFrame->data[0], data, fieldSize);

        this->time_ += this->pathStep_;
    }

private:

    using Raw = 
        Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    using VideoFrame =
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    using Map = Eigen::Map<VideoFrame, 0, Eigen::OuterStride<>>;

    using ColorMap = tau::ColorMap<tau::RgbMatrix<uint8_t>>;

    struct Point
    {
        int x;
        int y;
    };

    Point GetNextPath()
    {
        double x = this->radius_ * std::cos(this->time_);  
        double y = this->radius_ * std::sin(this->time_);

        Point result{
            static_cast<int>(std::round(x)) + this->center_.x,
            static_cast<int>(std::round(y)) + this->center_.y};

        return result;
    }

    void ComputeRawDistances_(Point point)
    {
        for (int i = 0; i < this->height_; ++i)
        {
            for (int j = 0; j < this->width_; ++j)
            {
                this->raw_(i, j) = static_cast<size_t>(
                    GetDistance(j - point.x, i - point.y));
            }
        }
    }

    template<typename T>
    static T GetDistance(T x, T y)
    {
        double x_ = static_cast<double>(x);
        double y_ = static_cast<double>(y);

        return static_cast<T>(std::ceil(std::sqrt(x_ * x_ + y_ * y_)));
    }


private:
    double time_;
    double pathStep_;
    int height_;
    int width_;
    Point center_;
    int radius_;
    Raw raw_;
    ColorMap colorMap_;
};


} // end namespace clip
