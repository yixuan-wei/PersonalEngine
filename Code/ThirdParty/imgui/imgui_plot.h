#pragma once

#include "ThirdParty/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ThirdParty/imgui/imgui_internal.h"
#include <string>

namespace ImGui{
    static ImU32 InvertColorU32(ImU32 in);
    static void PlotMultiEx(ImGuiPlotType plot_type, const char* label, int num_datas, const char** names, const ImColor* colors, 
        float(*getter)(const void* data, int idx), const void* const* datas, int values_count, float scale_min, float scale_max, 
        ImVec2 graph_size, const char** intervalNames);
    void PlotMultiLines(const char* label, int num_datas, const char** names, const ImColor* colors, 
        float(*getter)(const void* data, int idx), const void* const* datas, int values_count, float scale_min, 
        float scale_max, ImVec2 graph_size, const char** intervalNames);

    static void PlotMultiEx(ImGuiPlotType plot_type, const char* label, int num_datas, const std::string* names, const ImColor* colors,
        float(*getter)(const void* data, int idx), const void* const* datas, int values_count, float scale_min, float scale_max,
        ImVec2 graph_size, const char** intervalNames);
    void PlotMultiLines(const char* label, int num_datas, const std::string* names, const ImColor* colors,
        float(*getter)(const void* data, int idx), const void* const* datas, int values_count, float scale_min,
        float scale_max, ImVec2 graph_size, const char** intervalNames);
}