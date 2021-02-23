#pragma once

#include <string>

enum eBufferFormatType 
{
    BUFFER_FORMAT_VEC2,
    BUFFER_FORMAT_VEC3,
    BUFFER_FORMAT_VEC4,
    BUFFER_FORMAT_R8G8B8A8_UNORM,
};

struct buffer_attribute_t 
{
    std::string name;
    eBufferFormatType type = BUFFER_FORMAT_VEC2;
    unsigned int offset   = 0;

    buffer_attribute_t(char const* n, eBufferFormatType t, unsigned int o);
    buffer_attribute_t() {}
};