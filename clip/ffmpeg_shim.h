#pragma once


#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif


#ifdef _WIN32

#define FFMPEG_SHIM_PUSH_IGNORES \
    __pragma(warning(push)) \
    __pragma(warning(disable:4242)) \
    __pragma(warning(disable:4244))

#define FFMPEG_SHIM_POP_IGNORES \
    __pragma(warning(pop))

#else

#define FFMPEG_SHIM_PUSH_IGNORES
#define FFMPEG_SHIM_POP_IGNORES

#endif
