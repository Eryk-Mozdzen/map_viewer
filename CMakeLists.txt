cmake_minimum_required(VERSION 3.25)

project(map_viewer)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(STB REQUIRED stb)

include(FetchContent)
FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.92.7
)
FetchContent_MakeAvailable(imgui)

add_executable(${PROJECT_NAME}
	"main.cpp"
	"server.cpp"
	"server_osm.cpp"
	"server_bhmw.cpp"
	"${imgui_SOURCE_DIR}/imgui.cpp"
	"${imgui_SOURCE_DIR}/imgui_draw.cpp"
	"${imgui_SOURCE_DIR}/imgui_tables.cpp"
	"${imgui_SOURCE_DIR}/imgui_widgets.cpp"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"include"
	"${imgui_SOURCE_DIR}"
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-O3
	-Wall
	-Wextra
	-Wpedantic
	-std=c++20
)

target_link_libraries(${PROJECT_NAME}
	m
    glfw
    OpenGL::GL
	CURL::libcurl
	stb
)
