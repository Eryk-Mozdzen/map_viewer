#include <cmath>
#include <iostream>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "server_bhmw.hpp"
#include "server_osm.hpp"

ImVec2 operator+(const ImVec2 &v1, const ImVec2 v2) {
    return ImVec2(v1.x + v2.x, v1.y + v2.y);
}

ImVec2 operator-(const ImVec2 &v1, const ImVec2 v2) {
    return ImVec2(v1.x - v2.x, v1.y - v2.y);
}

ImVec2 operator*(const double &num, const ImVec2 v) {
    return ImVec2(num * v.x, num * v.y);
}

int main() {
    if(!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 800, "Map Viewer", nullptr, nullptr);
    if(window == nullptr) {
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImVec2 offset(-143856, -83068);
    int zoom = 10;
    bool use_osm = true;
    bool use_bhmw = true;

    server_osm osm;
    server_bhmw bhmw;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("window", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        ImGui::Text("offset x %f", offset.x);
        ImGui::Text("offset y %f", offset.y);
        ImGui::Text("zoom %d", zoom);
        ImGui::Checkbox("use OSM", &use_osm);
        ImGui::Checkbox("use BHMW", &use_bhmw);

        const ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
        const ImVec2 canvas_p1 = canvas_p0 + canvas_sz;

        constexpr int tile = 256;

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::InvisibleButton("canvas", canvas_sz,
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }
        if((io.MouseWheel > 0) && (zoom < 20)) {
            offset.x *= 2;
            offset.y *= 2;
            zoom++;
        }
        if((io.MouseWheel < 0) && (zoom > 0)) {
            zoom--;
            offset.x /= 2;
            offset.y /= 2;
        }

        ImDrawList *drawer = ImGui::GetWindowDrawList();
        drawer->PushClipRect(canvas_p0, canvas_p1, true);
        for(int y = (static_cast<int>(-offset.y) / tile);
            y <= (static_cast<int>(canvas_sz.y - offset.y) / tile); y++) {
            for(int x = (static_cast<int>(-offset.x) / tile);
                x <= (static_cast<int>(canvas_sz.x - offset.x) / tile); x++) {
                if(use_osm) {
                    osm.draw(*drawer, canvas_p0 + offset + tile * ImVec2(x, y), zoom, x, y);
                }
                if(use_bhmw) {
                    bhmw.draw(*drawer, canvas_p0 + offset + tile * ImVec2(x, y), zoom, x, y);
                }
            }
        }
        drawer->PopClipRect();

        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(255, 255, 255, 255);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
