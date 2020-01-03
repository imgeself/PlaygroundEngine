#pragma once

#include <PGSystem.h>

void DrawLogWindow() {
    if (!ImGui::Begin("Logs"))
    {
        ImGui::End();
        return;
    }

    static ImGuiTextFilter textFilter;
    static bool autoScroll = true;
    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    textFilter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        PGLog::ClearLogList();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    LogList logList = PGLog::GetLogList();
    for (LogItem item : logList) {
        if (!textFilter.PassFilter(item.logData)) {
            continue;
        }

        switch (item.type) {
        case LOG_WARNING:
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
        } break;
        case LOG_ERROR:
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        } break;
        default:
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } break;
        }

        ImGui::TextUnformatted(item.logData);
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleVar();

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

