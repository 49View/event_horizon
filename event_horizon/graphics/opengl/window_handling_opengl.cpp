#include "window_handling_opengl.hpp"
#include "gl_headers.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <graphics/window_handling.hpp>

namespace WindowHandling {

    GLFWwindow* window = nullptr;
    bool bUseGLFWPoll = true;

    void setDropCallback( GLFWdropfun fn ) {
        glfwSetDropCallback( window, fn );
    }

    void setResizeWindowCallback( GLFWwindowsizefun fn ) {
        glfwSetWindowSizeCallback( window, fn );
    }

    void setResizeFramebufferCallback( GLFWframebuffersizefun fn ) {
        glfwSetFramebufferSizeCallback( window, fn );
    }

    void gatherMainScreenInfo() {
        int w,h;
        glfwGetFramebufferSize( window, &w, &h );
        Vector2i sizei{ w, h };
        AppGlobals::getInstance().setScreenSizef( Vector2f{w,h});
        AppGlobals::getInstance().setScreenSizei( sizei );
        LOGR( "Gather Screen Info, Default framebuffer index: - size: [%d, %d]", sizei.x(), sizei.y() );
    }

    bool shouldWindowBeClosed() {
        return glfwWindowShouldClose( window ) != 0;
    }

    void initImGUI() {
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        ImGui::GetIO().FontGlobalScale = 0.5f;
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
    }

    void imguiUpdate() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    }

    void imguiUpdateStart() {
        ImGui::NewFrame();
    }

    void imguiUpdateEnd() {
        ImGui::Render();
    }

    void enableInputCallbacks() {
        bUseGLFWPoll = true;
        ImGui_ImplGlfw_InstallCallbacks( window );
    }

    void disableInputCallbacks() {
        bUseGLFWPoll = false;
        ImGui_ImplGlfw_DisableCallbacks( window );
    }

    void flush() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

}
