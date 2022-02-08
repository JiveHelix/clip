extern "C"
{

#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>

}

#include <iostream>


int main()
{
    const AVCodec *avCodec;
    void *iterator = 0;

    while ((avCodec = av_codec_iterate(&iterator)))
    {
        if (av_codec_is_encoder(avCodec) 
                && (avCodec->type == AVMEDIA_TYPE_VIDEO))
        {
            std::cout << avCodec->name << std::endl;
        }
    }

    AVCodec *p = avcodec_find_encoder_by_name("libx264");

    if (!p)
    {
        std::cerr << "Cannot find libx264" << std::endl;
        return 1;
    }

    return 0;
}


/*

mov_write_packet
    mov_write_single_packet 
        ff_mov_write_packet

mov_write_subtitle_end_packet
    mov_write_single_packet 
        ff_mov_write_packet

mov_write_squashed_packet
    mov_write_single_packet 
        ff_mov_write_packet

mov_write_header
    mov_create_chapter_track
        ff_mov_write_packet

mov_write_trailer
    mov_create_chapter_track
        ff_mov_write_packet

mov_write_header
    mov_create_timecode_track
        ff_mov_write_packet








   */
