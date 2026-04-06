#ifndef WMS_HPP
#define WMS_HPP

#include <atomic>
#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <thread>
#include <tuple>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "queue.hpp"

class wms {
    const std::filesystem::path cache_directory;
    queue<std::tuple<int, int, int>> download_queue;
    std::jthread download_thread;
    std::map<std::tuple<int, int, int>, GLuint> textures;
    std::atomic_bool cache_clear;

    virtual std::string generate_url(const int tile_zoom, const int tile_x, const int tile_y) = 0;

public:
    constexpr static int tile_size = 256;

    wms(const std::filesystem::path cache_directory,
        const std::chrono::milliseconds minimal_request_period = std::chrono::milliseconds(0),
        const std::string user_agent = "app");
    ~wms();

    void draw(ImDrawList *drawer,
              const int screen_x,
              const int screen_y,
              const int tile_zoom,
              const int tile_x,
              const int tile_y);
    void clear_cache();
};

#endif
