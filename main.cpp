#include <cmath>
#include <iostream>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "server.hpp"

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

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImVec2 offset(0.0f, 0.0f);
    int zoom = 3;
    server serv("cache");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            continue;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");

        const ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
        const ImVec2 canvas_p1 = canvas_p0 + canvas_sz;

        constexpr int tile = 256;
        const int tiles = 1 << zoom;

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::InvisibleButton("canvas", canvas_sz,
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            offset.x += io.MouseDelta.x;
            offset.y += io.MouseDelta.y;
        }
        if((io.MouseWheel > 0) && (zoom < 20)) {
            offset.x -= tile * (1 << zoom) / 2;
            offset.y -= tile * (1 << zoom) / 2;
            zoom++;
        }
        if((io.MouseWheel < 0) && (zoom > 0)) {
            zoom--;
            offset.x += tile * (1 << zoom) / 2;
            offset.y += tile * (1 << zoom) / 2;
        }

        ImDrawList *drawer = ImGui::GetWindowDrawList();
        drawer->PushClipRect(canvas_p0, canvas_p1, true);
        for(int row = (static_cast<int>(-offset.y) / tile);
            row <= (static_cast<int>(canvas_sz.y - offset.y) / tile); row++) {
            for(int col = (static_cast<int>(-offset.x) / tile);
                col <= (static_cast<int>(canvas_sz.x - offset.x) / tile); col++) {
                if((row >= 0) && (row < tiles) && (col >= 0) && (col < tiles)) {
                    /*drawer->AddRect(canvas_p0 + offset + tile * ImVec2(col + 0, row + 0),
                                    canvas_p0 + offset + tile * ImVec2(col + 1, row + 1),
                                    IM_COL32(255, 255, 255, 255), 0, ImDrawFlags_None, 1);
                    char buf[64];
                    sprintf(buf, "(%d, %d)", row, col);
                    drawer->AddText(canvas_p0 + offset + tile * ImVec2(col, row),
                                    IM_COL32(255, 255, 255, 255), buf);*/

                    serv.draw(*drawer, canvas_p0 + offset + tile * ImVec2(col, row), zoom, col,
                              row);
                }
            }
        }
        drawer->PopClipRect();

        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 0);
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
