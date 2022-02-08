/**
 * @author Jive Helix (jivehelix@gmail.com)
 * @copyright 2022 Jive Helix
 * Licensed under the MIT license. See LICENSE file.
 */
#include <catch2/catch.hpp>

#include "clip/channel_layout.h"

TEST_CASE("Stereo has two channels", "[channel_layout]")
{
    clip::ChannelLayout channelLayout(AV_CH_LAYOUT_STEREO);
    REQUIRE(channelLayout.GetChannelCount() == 2);
}
