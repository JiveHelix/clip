/**
  * @file sample_traits.h
  * 
  * @brief Inspect types used to represent audio samples.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include <type_traits>


namespace clip
{


template<typename T, typename Enable = void>
struct SampleTraits {};

template<typename T>
struct SampleTraits<T, std::enable_if_t<std::is_integral_v<T>>>
{
    using type = T;
    static constexpr T minimum = std::numeric_limits<T>::lowest();
    static constexpr T maximum = std::numeric_limits<T>::max();
    static constexpr T range = (maximum - minimum) / 2;
    static constexpr T midpoint = minimum + range;
};

template<typename T>
struct SampleTraits<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
    using type = T;
    static constexpr T minimum = static_cast<T>(-1.0);
    static constexpr T maximum = static_cast<T>(1.0);
    static constexpr T range = (maximum - minimum) / 2;
    static constexpr T midpoint = static_cast<T>(0);
};
    

template<typename Traits, typename F>
std::enable_if_t<std::is_floating_point_v<F>, typename Traits::type>
Scale(F value)
{
    using T = typename Traits::type;

    if constexpr (std::is_floating_point_v<T>)
    {
        return static_cast<T>(value);
    }

    return static_cast<T>(
        static_cast<F>(Traits::midpoint)
        + std::round(value * static_cast<F>(Traits::range)));
}


} // end namespace clip
