#pragma once



#include <fields/fields.h>
#include <pex/select.h>
#include <pex/selectors.h>
#include <vector>
#include <tau/size.h>


namespace clip
{


struct Resolution
{
    int width;
    int height;

    tau::Size<int> ToSize() const
    {
        return {this->width, this->height};
    }

    static constexpr auto fields = std::make_tuple(
        fields::Field(&Resolution::width, "width"),
        fields::Field(&Resolution::height, "height"));

    static constexpr auto fieldsTypeName = "Resolution";
};


DECLARE_EQUALITY_OPERATORS(Resolution)
DECLARE_OUTPUT_STREAM_OPERATOR(Resolution)


static constexpr Resolution sd{640, 480};
static constexpr Resolution hd{1920, 1080};
static constexpr Resolution uhd4k{3840, 2160};
static constexpr Resolution uhd8k{7680, 4320};


struct ResolutionChoices
{
    using Type = Resolution;

    static std::vector<Resolution> GetChoices()
    {
        return {sd, hd, uhd4k, uhd8k};
    }
};


struct ResolutionConverter
{
    static std::string ToString(const Resolution &resolution)
    {
        std::string description;

        if (resolution == sd)
        {
            description = "sd ";
        }
        else if (resolution == hd)
        {
            description = "hd ";
        }
        else if (resolution == uhd4k)
        {
            description = "uhd4k ";
        }
        else if (resolution == uhd8k)
        {
            description = "uhd8k ";
        }
        else
        {
            description = "";
        }

        return fmt::format(
            "{}({}x{})",
            description,
            resolution.width,
            resolution.height);
    }
};


using ResolutionSelect = pex::MakeSelect<ResolutionChoices, pex::GetTag>;
using ResolutionControl = pex::ControlSelector<ResolutionSelect>;


}
