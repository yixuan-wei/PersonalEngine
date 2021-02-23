#include "Engine/Renderer/buffer_attribute_t.hpp"

//////////////////////////////////////////////////////////////////////////
buffer_attribute_t::buffer_attribute_t(char const* n, eBufferFormatType t, unsigned int o)
    :name(n)
    ,type(t)
    ,offset(o)
{
}

