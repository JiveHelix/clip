
#include <cstdlib>
#include <iostream>
#include <optional>

#include "clip/ffmpeg_shim.h"

FFMPEG_SHIM_PUSH_IGNORES
extern "C"
{

#include <libavformat/avformat.h>
#include <libavutil/dict.h>

}
FFMPEG_SHIM_POP_IGNORES

#include <clip/error.h>



int main(int count, char **args)
{
    if (count != 2)
    {
        std::cerr << "Usage: " << args[0] << " file.avi" << std::endl;
        return EXIT_FAILURE;
    }

    AVFormatContext *context = NULL;

    int result = avformat_open_input(&context, args[1], NULL, NULL);

    if (result != 0)
    {
        std::cerr << clip::DescribeError("Failed to open file", result)
            << std::endl;

        return EXIT_FAILURE;
    }

    result = avformat_find_stream_info(context, NULL);

    if (result < 0)
    {
        std::cerr << clip::DescribeError("Failed to find stream info", result)
            << std::endl;

        return EXIT_FAILURE;
    }

    std::optional<size_t> videoStream;

    for (size_t i = 0; i < context->nb_streams; ++i)
    {
        if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }

    if (!videoStream)
    {
        std::cerr << "Unable to find video stream" << std::endl;
        return EXIT_FAILURE;
    }

    AVCodecParameters *codecParameters =
        context->streams[*videoStream]->codecpar;

    if (!codecParameters)
    {
        std::cerr << "Codec not found" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "width: " << codecParameters->width << std::endl;
    std::cout << "height: " << codecParameters->height << std::endl;

    std::cout << avcodec_get_name(codecParameters->codec_id) << std::endl;
    std::cout << "AVPixelFormat: " << codecParameters->format << std::endl;

#if 1
    AVPacket packet{};

    size_t frameCount = 0;

    while (true)
    {
        result = av_read_frame(context, &packet);

        if (result != 0)
        {
            std::cerr << clip::DescribeError("Read frame", result)
                << std::endl;

            break;
        }

        size_t frameDataSize = packet.buf->size;

        std::cout << "pts: " << packet.pts << std::endl;

        std::cout << std::dec << frameCount << " frame has " << frameDataSize
            << " bytes at " << static_cast<void *>(packet.buf->data)
            << " buffer at " << static_cast<void *>(packet.buf->buffer)
            << std::endl;

        ++frameCount;
    }
#endif

    std::cout << "iformat->name: " << context->iformat->name << std::endl;

    std::cout << "iformat->long_name: "
        << context->iformat->long_name << std::endl;

    if (context->iformat->mime_type)
    {
        std::cout << "iformat->mime_type: "
            << context->iformat->mime_type << std::endl;
    }

    std::cout << "metadata: " << std::endl;

    const AVDictionaryEntry *entry = av_dict_iterate(context->metadata, NULL);

    while (entry != NULL)
    {
        std::cout << "key: " << entry->key
            << ", value: " << entry->value << std::endl;

        entry = av_dict_iterate(context->metadata, entry);
    }

    avformat_close_input(&context);

    return EXIT_SUCCESS;
}
