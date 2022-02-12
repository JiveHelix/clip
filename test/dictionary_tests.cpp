/**
 * @author Jive Helix (jivehelix@gmail.com)
 * @copyright 2022 Jive Helix
 * Licensed under the MIT license. See LICENSE file.
 */

#include <catch2/catch.hpp>
#include "clip/dictionary.h"


TEST_CASE("Add value to dictionary", "[dictionary]")
{
    clip::Dictionary options;
    std::string key("preset");
    std::string value("veryfast");
    options.Set(key, value);
    REQUIRE(value == options[key]);
}


TEST_CASE("Bad key throws", "[dictionary]")
{
    clip::Dictionary options;

    REQUIRE_THROWS_AS(
        options["preset"],
        std::out_of_range);
}
