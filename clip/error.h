/**
  * @file error.h
  * 
  * @brief Exceptions thrown by this library.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


extern "C"
{

#include <libavutil/error.h>

}

#include "jive/create_exception.h"


namespace clip
{


CREATE_EXCEPTION(ClipError, std::runtime_error);
CREATE_EXCEPTION(VideoError, ClipError);
CREATE_EXCEPTION(AudioError, ClipError);

// Report an error from one of ffmpeg's library functions.
CREATE_EXCEPTION(FfmpegError, ClipError);


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
