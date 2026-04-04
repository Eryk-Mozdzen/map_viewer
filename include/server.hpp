#ifndef SERVER_HPP
#define SERVER_HPP

#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <thread>
#include <tuple>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "queue.hpp"

class server {
    const std::filesystem::path cache_directory;
    queue<std::tuple<int, int, int>> download_queue;
    std::jthread download_thread;
    std::map<std::tuple<int, int, int>, GLuint> textures;

    virtual std::string generate_url(const int zoom, const int x, const int y) = 0;

public:
    server(const std::filesystem::path cache_directory,
           const std::chrono::milliseconds minimal_request_period = std::chrono::milliseconds(0),
           const std::string user_agent = "app");
    ~server();

    void draw(ImDrawList &drawer, const ImVec2 position, const int zoom, const int x, const int y);
};

#endif
