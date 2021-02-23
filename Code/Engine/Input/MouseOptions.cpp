#include "Engine/Input/MouseOptions.hpp"

//////////////////////////////////////////////////////////////////////////
MouseOptions::MouseOptions(eMousePositionMode mode, bool visible /*= true*/, bool clipped /*= true*/)
    :moveMode(mode)
    ,isVisible(visible)
    ,isClipped(clipped)
{
}
