#include <cmath>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

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

    ImVec2 scrolling(0.0f, 0.0f);

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
        const ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::InvisibleButton("canvas", canvas_sz,
                               ImGuiButtonFlags_MouseButtonLeft |
                                   ImGuiButtonFlags_MouseButtonRight);
        const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y);
        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

        if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right, -1)) {
            scrolling.x += io.MouseDelta.x;
            scrolling.y += io.MouseDelta.y;
        }

        ImDrawList *drawer = ImGui::GetWindowDrawList();
        drawer->PushClipRect(canvas_p0, canvas_p1, true);
        constexpr float GRID_STEP = 64.0f;
        for(float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP) {
            drawer->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y),
                            ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        }
        for(float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP) {
            drawer->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y),
                            ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
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
