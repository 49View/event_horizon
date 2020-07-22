#include "window_handling_opengl_glfw.hpp"
#include <graphics/opengl/gl_headers.hpp>
#ifdef _USE_IMGUI_
#include <graphics/imgui/imgui.h>
#include <graphics/opengl/GLFW/imgui_impl_glfw.h>
#include <graphics/opengl/imgui_impl_opengl3.h>
#endif

#include <graphics/window_handling.hpp>
#include <graphics/render_list.h>
#include <core/default_font.hpp>

//#define _USE_IMGUI_

namespace WindowHandling {

    GLFWwindow* window = nullptr;
    bool bUseGLFWPoll = true;
    bool mousePassThrough = true;
    bool keyboardPassThrough = true;

    void setKeyCallback( GLFWkeyfun fn ) {
        glfwSetKeyCallback( window, fn );
    }

    void setDropCallback( GLFWdropfun fn ) {
        glfwSetDropCallback( window, fn );
    }

    void setResizeWindowCallback( GLFWwindowsizefun fn ) {
        glfwSetWindowSizeCallback( window, fn );
    }

    void setResizeFramebufferCallback( GLFWframebuffersizefun fn ) {
        glfwSetFramebufferSizeCallback( window, fn );
    }

    bool isMouseInputActive() {
        return mousePassThrough;
    }

    bool isKeyboardInputActive() {
        return keyboardPassThrough;
    }

    void gatherMainScreenInfo() {
        int w,h;
        glfwGetFramebufferSize( window, &w, &h );
        Vector2i sizei{ w, h };
        AppGlobals::getInstance().setScreenSizef( Vector2f{w,h});
        AppGlobals::getInstance().setScreenSizei( sizei );
    }

    bool shouldWindowBeClosed() {
        return glfwWindowShouldClose( window ) != 0;
    }

    void initImGUI() {
#ifdef _USE_IMGUI_
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        static SerializableContainer defaultFont = getDefaultFont();
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(defaultFont.data(), defaultFont.size(), 30.0f);
        ASSERT(font);

        ImGui::GetStyle().FrameRounding = 2.0f;
        ImGui::GetStyle().GrabRounding = 2.0f;
        ImGui::GetStyle().FramePadding = ImVec2(4, 2);
        ImGui::GetStyle().ItemSpacing = ImVec2(10, 2);
        ImGui::GetStyle().IndentSpacing = 12;
        ImGui::GetStyle().ScrollbarSize = 10;

        ImGui::GetStyle().WindowRounding = 4;
        ImGui::GetStyle().FrameRounding = 4;
        ImGui::GetStyle().PopupRounding = 4;
        ImGui::GetStyle().ScrollbarRounding = 6;
        ImGui::GetStyle().GrabRounding = 4;
        ImGui::GetStyle().TabRounding = 4;

        ImGui::GetStyle().DisplaySafeAreaPadding = ImVec2(4, 4);

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

//        ImGui::GetIO().FontGlobalScale = 1.5f;
//        ImGui::GetIO().FontAllowUserScaling = true;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
        const char* glsl_version = "#version 300 es";
#else
        const char* glsl_version = "#version 410";
#endif
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Setup style
        ImGui::StyleColorsDark();
#endif
    }

    void preUpdate() {
#ifdef _USE_IMGUI_
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        mousePassThrough = !io.WantCaptureMouse;
        keyboardPassThrough = !io.WantCaptureKeyboard || !io.WantTextInput;
#endif
    }

    void enableInputCallbacks() {
        bUseGLFWPoll = true;
#ifdef _USE_IMGUI_
//        ImGui_ImplGlfw_InstallCallbacks( window );
#endif
    }

    void disableInputCallbacks() {
        bUseGLFWPoll = false;
#ifdef _USE_IMGUI_
//        ImGui_ImplGlfw_DisableCallbacks( window );
#endif
    }

    void flush() {
#ifdef _USE_IMGUI_
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        glfwSwapBuffers( window );
#ifdef __APPLE__
        static bool macMoved = false;

        if(!macMoved) {
            int x, y;
            glfwGetWindowPos(window, &x, &y);
            glfwSetWindowPos(window, ++x, y);
            macMoved = true;
        }
#endif
    }

    void pollEvents() {
        //glfwWaitEvents();
        if ( bUseGLFWPoll ) {
            glfwPollEvents();
        }
    }

    void enableVSync( const bool val ) {
        glfwSwapInterval( val != 0 ? 1 : 0 );
    }

    bool isInputEnabled() {
        return bUseGLFWPoll;
    }

    void enableMouseCursor( bool flag ) {
        glfwSetInputMode(window, GLFW_CURSOR, flag ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void imRenderLoopStats( const RenderStats& rs ) {
#ifdef _USE_IMGUI_
        ImGui::Begin("Renderer Console");
        ImGui::Text("Application average %.3f", 1000.0f / ImGui::GetIO().Framerate );
        ImGui::Text("Current FrameRate (%.1f FPS)", ImGui::GetIO().Framerate );
        ImGui::Text("Number drawcalls: %d", rs.getDrawCallsPerFrame() );
        ImGui::Text("Number render materials: %d", rs.getInMemoryMaterials() );
        ImGui::Text("Number vertex buffers: %d", rs.getInMemoryVertexBuffers() );
        ImGui::Text("Number textures: %d", rs.getInMemoryTextures() );
        ImGui::End();
#endif
    }
}
