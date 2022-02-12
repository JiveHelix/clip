/**
  * @file circle_gradient.h
  * 
  * @brief Creates a moving circle gradient.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

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


class CircleGradient
{
public:
    using ColorMap = tau::ColorMap<tau::RgbMatrix<uint8_t>>;
    using Output = ColorMap::Colors;

    static_assert(
        tau::MatrixTraits<Output>::columns != Eigen::Dynamic,
        "Expected compile-time column count");

    CircleGradient(const VideoOptions &options)
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

    Eigen::Index GetDataWidth() const
    {
        return this->width_ * tau::MatrixTraits<Output>::columns;
    }

    void Reset()
    {
        this->time_ = 0.0;
    }

    void FillFrame(Output *output)
    {
        this->ComputeRawDistances_(this->GetNextPath());
        this->colorMap_(this->raw_, output);
        this->time_ += this->pathStep_;
    }

private:

    using Raw = 
        Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

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
