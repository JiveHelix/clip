#pragma once

extern "C"
{

#include <libavutil/error.h>

}


#include "jive/create_exception.h"


namespace clip
{


CREATE_EXCEPTION(VideoError, std::runtime_error);

inline std::string AvErrorToString(int errorNumber)
{
    char buffer[AV_ERROR_MAX_STRING_SIZE];
    int result = av_strerror(errorNumber, &buffer[0], AV_ERROR_MAX_STRING_SIZE);

    if (result < 0)
    {
        return "Unknown error";
    }

    // Copy the local buffer to the std::string return value
    return &buffer[0];
}


inline std::string DescribeError(const std::string &message, int errorNumber)
{
    return message + ": " + AvErrorToString(errorNumber);
}


} // end namespace clip
