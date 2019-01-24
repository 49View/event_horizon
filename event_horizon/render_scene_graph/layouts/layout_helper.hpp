//
// Created by Dado on 2019-01-23.
//

#pragma once

struct LayoutImGuiShowAndHide {
    static bool bShowCamera;
    static bool bShowCloudGeom;
    static bool bShowCloudMaterial;
    static bool bShowConsole;
    static bool bShowSceneGraph;
    static bool bShowImage;
    static bool bShowLogin;
    static bool bShowMaterial;
    static bool bShowTimeline;
};

#define LAYOUT_IMGUI_WINDOW_POSSIZE( flag , _r) \
    static bool firstShow = true; \
    if ( LayoutImGuiShowAndHide::bShow##flag && firstShow ) { \
        ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } ); \
        ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } ); \
        firstShow = false; \
    } \
    if ( LayoutImGuiShowAndHide::bShow##flag ) { \
        if ( !ImGui::Begin( #flag, &LayoutImGuiShowAndHide::bShow##flag )) { \
            ImGui::End(); \
            return; \
        } \
    } else { \
        return; \
    }
