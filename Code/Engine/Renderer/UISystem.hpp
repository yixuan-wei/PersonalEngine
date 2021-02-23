#pragma once

class RenderContext;
class Window;

class UISystem
{
public:
    void Startup(RenderContext* rtx, Window* wind);
    void BeginFrame();
    void Render();
    void Shutdown();
    
};