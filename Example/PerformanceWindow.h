#pragma once

#include <PGProfiler.h>
#include <PGSystem.h>

static bool CompareProfile(const ProfileData& left, const ProfileData& right)
{
    return (left.startNanoSeconds < right.startNanoSeconds);
}

struct ProfileNode {
    ProfileData data;
    std::vector<ProfileNode> childNodes;
};

static bool InsertProfileData(std::vector<ProfileNode>& profileList, const ProfileData& data) {
    bool inserted = false;
    for (ProfileNode& node : profileList) {
        uint64_t nodeStartTime = node.data.startNanoSeconds;
        uint64_t nodeEndTime = node.data.startNanoSeconds + node.data.durationNanoSeconds;

        if (data.startNanoSeconds >= nodeStartTime && data.startNanoSeconds < nodeEndTime) {
            if (!node.childNodes.empty()) {
                inserted = InsertProfileData(node.childNodes, data);
            }
            if (!inserted) {
                ProfileNode newNode;
                newNode.data = data;
                node.childNodes.push_back(newNode);
                inserted = true;
            }
        }
    }
    if (!inserted) {
        ProfileNode newNode;
        newNode.data = data;
        profileList.push_back(newNode);
        inserted = true;
    }
    return inserted;
}

static void DrawChildNodes(const std::vector<ProfileNode>& profileTree) {
    for (auto& profileNode : profileTree) {
        const char* profileName = profileNode.data.name.c_str();
        double durationMilliseconds = profileNode.data.durationNanoSeconds / 1000000.0;
        if (profileNode.childNodes.empty()) {
            ImGui::Text("%s %.3f ms", profileName, durationMilliseconds);
        } else {
            //ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode(profileName, "%s %.3f ms", profileName, durationMilliseconds)) {
                DrawChildNodes(profileNode.childNodes);
                ImGui::TreePop();
            }
        }
    }
}

static void DrawPerformanceWindow() {
    ProfileList profileList = PGProfileTimer::s_ProfileResults;
    std::sort(profileList.begin(), profileList.end(), CompareProfile);

    // Put profile data into a tree-like structure, so that we can look profile data in a hierarchical view
    std::vector<ProfileNode> profileTree;
    for (ProfileData profileData : profileList) {
        InsertProfileData(profileTree, profileData);
    }

    if (ImGui::Begin("Performance")) {
        DrawChildNodes(profileTree);
    }
    ImGui::End();
}

