#pragma once

enum eMousePositionMode {
    MOUSE_ABSOLUTE,
    MOUSE_RELATIVE
};


struct MouseOptions
{
    bool isClipped = true;
    bool isVisible = true;
    eMousePositionMode moveMode = eMousePositionMode::MOUSE_ABSOLUTE;

    MouseOptions(eMousePositionMode mode, bool visible = true, bool clipped = true);
};