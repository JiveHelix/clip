/**
  * @file dictionary.h
  * 
  * @brief Wrapper around AVDictionary.
  * 
  * @author Jive Helix (jivehelix@gmail.com)
  * @date 11 Feb 2022
  * @copyright Jive Helix
  * Licensed under the MIT license. See LICENSE file.
**/

#pragma once


#include "clip/ffmpeg_shim.h"
FFMPEG_SHIM_PUSH_IGNORES
extern "C"
{

#include <libavutil/dict.h>

}
FFMPEG_SHIM_POP_IGNORES


#include "clip/error.h"


namespace clip
{


class Dictionary
{
public:
    Dictionary(): object_(NULL) {}

    Dictionary(const Dictionary &other)
        : object_(NULL)
    {
        if (0 != av_dict_copy(&this->object_, other.object_, 0))
        {
            throw VideoError("Failed to copy AVDictionary");
        }
    }

    Dictionary(Dictionary &&other)
        : object_(other.object_)
    {
        other.object_ = NULL;
    }

    Dictionary & operator=(const Dictionary &other)
    {
        if (this->object_)
        {
            av_dict_free(&this->object_);
        }

        if (0 != av_dict_copy(&this->object_, other.object_, 0))
        {
            throw VideoError("Failed to copy AVDictionary");
        }

        return *this;
    }

    Dictionary & operator=(Dictionary &&other)
    {
        if (this->object_)
        {
            av_dict_free(&this->object_);
        }

        this->object_ = other.object_;
        other.object_ = NULL;

        return *this;
    }

    ~Dictionary()
    {
        if (this->object_)
        {
            av_dict_free(&this->object_);
        }
    }

    AVDictionary ** Get()
    {
        return &this->object_;
    }

    void Set(const char *key, const char *value)
    {
        int result = av_dict_set(&this->object_, key, value, 0); 

        if (result != 0)
        {
            throw VideoError("Failed to set key/value.");
        }
    }

    void Set(const std::string &key, const std::string &value)
    {
        this->Set(key.c_str(), value.c_str());
    }

    std::string operator[](const std::string &key)
    {
        AVDictionaryEntry *entry =
            av_dict_get(this->object_, key.c_str(), NULL, 0);

        if (entry)
        {
            return {entry->value};
        }
        else
        {
            throw std::out_of_range("Key not found");
        }
    }

private:
    AVDictionary *object_;
};


} // end namespace clip
