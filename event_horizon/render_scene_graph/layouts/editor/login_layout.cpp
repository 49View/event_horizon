//
// Created by Dado on 2018-10-29.
//

#include "login_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

void ImGuiLogin( Scene* p, const Rect2f& _r ) {
    std::string title = "Login";
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse );

    static char projectInputChar[256];
    static char emailInputChar[256];
    static char passwordInputChar[256];
    static std::string projectBuf;
    static std::string emailBuf;
    static std::string passwordBuf;
    projectBuf.reserve(256);
    if ( Http::project().size() < 256 && projectBuf.empty() ) {
        projectBuf = Http::project();
        strcpy( projectInputChar, projectBuf.data() );
    }

    if ( emailBuf.empty() ) {
        emailBuf = Http::cachedLoginFields().email;
        strcpy( emailInputChar, emailBuf.data() );
    }

    if ( passwordBuf.empty() ) {
        passwordBuf = Http::cachedLoginFields().password;
        strcpy( passwordInputChar, passwordBuf.data() );
    }

    static LoginFields lf;
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Email")));
    ImGui::InputText( "email", emailInputChar, 256 );
    ImGui::PopID();
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Password")));
    ImGui::InputText( "password", passwordInputChar, 256, ImGuiInputTextFlags_Password );
    ImGui::PopID();
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Project")));
    ImGui::InputText( "Project", projectInputChar, 256 );
    ImGui::PopID();

    lf.email = emailInputChar;
    lf.password = passwordInputChar;
    lf.project = projectInputChar;

    if ( ImGui::Button( "Login" ) ) {
        Http::login( lf );
    }

    ImGui::End();
}
