#include <cmath>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "wms.hpp"
#include "wms_bhmw.hpp"
#include "wms_osm.hpp"

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

    ImVec2 offset(-144506, -83391);
    int zoom = 10;
    bool use_osm = true;
    bool use_bhmw = true;
    bool transparent_bhmw = true;

    wms_osm osm;
    wms_bhmw bhmw;

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
        ImGui::Checkbox("OSM enable", &use_osm);
        ImGui::Checkbox("BHMW enable", &use_bhmw);
        if(ImGui::Checkbox("BHMW transparent", &transparent_bhmw)) {
            bhmw.transparent(transparent_bhmw);
            bhmw.clear_cache();
        }

        const ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
        const ImVec2 canvas_p1 = canvas_p0 + canvas_sz;

        const ImGuiIO &io = ImGui::GetIO();
        const ImVec2 canvas_mouse = io.MousePos - canvas_p0 - (0.5 * canvas_sz);
        ImGui::InvisibleButton("canvas", canvas_sz,
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            offset = offset + io.MouseDelta;
        }
        if((io.MouseWheel > 0) && (zoom < 20)) {
            offset = (2 * offset) - canvas_mouse;
            zoom++;
        }
        if((io.MouseWheel < 0) && (zoom > 0)) {
            zoom--;
            offset = 0.5 * (offset + canvas_mouse);
        }

        ImDrawList *drawer = ImGui::GetWindowDrawList();
        drawer->PushClipRect(canvas_p0, canvas_p1, true);

        const int x0 = std::floor((-(canvas_sz.x / 2) - offset.x) / wms::tile_size);
        const int y0 = std::floor((-(canvas_sz.y / 2) - offset.y) / wms::tile_size);
        const int xn = std::ceil(canvas_sz.x / wms::tile_size);
        const int yn = std::ceil(canvas_sz.y / wms::tile_size);
        const int px = canvas_p0.x + (canvas_sz.x / 2) + offset.x + (wms::tile_size * x0);
        const int py = canvas_p0.y + (canvas_sz.y / 2) + offset.y + (wms::tile_size * y0);

        for(int y = 0; y <= yn; y++) {
            for(int x = 0; x <= xn; x++) {
                if(use_osm) {
                    osm.draw(px + (wms::tile_size * x), py + (wms::tile_size * y), zoom, x + x0,
                             y + y0);
                }
                if(use_bhmw) {
                    bhmw.draw(px + (wms::tile_size * x), py + (wms::tile_size * y), zoom, x + x0,
                              y + y0);
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
