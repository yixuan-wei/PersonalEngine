#include "Engine/Renderer/UISystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Platform/Window.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"

//////////////////////////////////////////////////////////////////////////
void UISystem::Startup(RenderContext* rtx, Window* wind)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(rtx->m_device, rtx->m_context);
    ImGui_ImplWin32_Init(wind->m_hwnd);
}

//////////////////////////////////////////////////////////////////////////
void UISystem::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();
}

//////////////////////////////////////////////////////////////////////////
void UISystem::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//////////////////////////////////////////////////////////////////////////
void UISystem::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
