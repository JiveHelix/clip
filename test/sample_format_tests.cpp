/**
 * @author Jive Helix (jivehelix@gmail.com)
 * @copyright 2022 Jive Helix
 * Licensed under the MIT license. See LICENSE file.
 */
#include <catch2/catch.hpp>

#include "clip/sample_format.h"


TEST_CASE("Check non planar formats", "[sample_format]")
{
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_U8>::isPlanar);
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_S16>::isPlanar);
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_S32>::isPlanar);
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_S64>::isPlanar);
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_FLT>::isPlanar);
    STATIC_REQUIRE_FALSE(clip::SampleFormat<AV_SAMPLE_FMT_DBL>::isPlanar);

    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_U8));
    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S16));
    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S32));
    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S64));
    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_FLT));
    REQUIRE(0 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_DBL));
}


TEST_CASE("Check planar formats", "[sample_format]")
{
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_U8P>::isPlanar);
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_S16P>::isPlanar);
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_S32P>::isPlanar);
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_S64P>::isPlanar);
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_FLTP>::isPlanar);
    STATIC_REQUIRE(clip::SampleFormat<AV_SAMPLE_FMT_DBLP>::isPlanar);

    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_U8P));
    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S16P));
    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S32P));
    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_S64P));
    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_FLTP));
    REQUIRE(1 == av_sample_fmt_is_planar(AV_SAMPLE_FMT_DBLP));
}
