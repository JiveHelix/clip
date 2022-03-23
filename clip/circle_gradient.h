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


#include <cmath>
#include "tau/angles.h"
#include "tau/color_map.h"
#include "tau/color_maps/turbo.h"


namespace clip
{


template<typename T>
class CircleGradient
{
public:
    using Values = 
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    CircleGradient(
            int height,
            int width,
            int framesPerSecond,
            double phase = 0.0)
        :
        time_(0.0),
        pathStep_(tau::Angles<double>::tau / (6 * framesPerSecond)),
        height_(height),
        width_(width),
        phase_(phase),
        center_{width / 2, height / 2},
        radius_(std::min(height, width) / 3),
        values_(height, width)
    {

    }

    int GetWidth() const
    {
        return this->width_;
    }

    int GetHeight() const
    {
        return this->height_;
    }

    void SetSize(int height, int width)
    {
        this->height_ = height;
        this->width_ = width;
        this->center_ = Point{width / 2, height / 2};
        this->radius_ = std::min(height, width) / 3;
        this->values_ = Values(height, width);
    }

    void Reset()
    {
        this->time_ = 0.0;
    }

    void Tick()
    {
        this->time_ += this->pathStep_;
    }

    Values GetNext()
    {
        this->ComputeRawDistances_(this->GetNextPath_());
        this->Tick();
        return this->values_;
    }

    template<typename U>
    static U GetDistance(U x, U y)
    {
        double x_ = static_cast<double>(x);
        double y_ = static_cast<double>(y);

        return static_cast<U>(std::ceil(std::sqrt(x_ * x_ + y_ * y_)));
    }

    T GetMaximumValue() const
    {
        return static_cast<T>(
            GetDistance(this->height_ / 2, this->width_ / 2) + this->radius_);
    }

private:
    struct Point
    {
        int x;
        int y;
    };

    Point GetNextPath_()
    {
        double x = this->radius_ * std::cos(this->time_ + this->phase_);  
        double y = this->radius_ * std::sin(this->time_ + this->phase_);

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
                this->values_(i, j) = static_cast<T>(
                    GetDistance(j - point.x, i - point.y));
            }
        }
    }

private:
    double time_;
    double pathStep_;
    int height_;
    int width_;
    double phase_;
    Point center_;
    int radius_;
    Values values_;
};


template<typename T>
class CircleGradientColors
{
public:
    using ColorMap = tau::ColorMap<tau::RgbMatrix<uint8_t>>;
    using Output = ColorMap::Colors;

    static_assert(
        tau::MatrixTraits<Output>::columns != Eigen::Dynamic,
        "Expected compile-time column count");

    CircleGradientColors(int height, int width, int framesPerSecond)
        :
        circleGradient_(height, width, framesPerSecond),
        colorMap_(
            tau::turbo::MakeRgb8(this->circleGradient_.GetMaximumValue()))
    {

    }

    Eigen::Index GetDataWidth() const
    {
        return this->circleGradient_.GetWidth()
            * tau::MatrixTraits<Output>::columns;
    }

    void Reset()
    {
        this->circleGradient_.Reset();
    }

    void FillFrame(Output *output)
    {
        this->colorMap_(this->circleGradient_.GetNext(), output);
    }

private:
    CircleGradient<T> circleGradient_;
    ColorMap colorMap_;
};


} // end namespace clip
