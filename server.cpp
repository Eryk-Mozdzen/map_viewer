#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <tuple>

#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <imgui.h>
#include <stb/stb_image.h>

#include "server.hpp"

static std::string get_filename(const std::tuple<int, int, int> coordinates) {
    return std::to_string(std::get<0>(coordinates)) + "_" +
           std::to_string(std::get<1>(coordinates)) + "_" +
           std::to_string(std::get<2>(coordinates)) + ".png";
}

server::server(const std::filesystem::path cache_directory, const std::string user_agent)
    : cache_directory{cache_directory} {
    if(!std::filesystem::exists(cache_directory)) {
        std::filesystem::create_directories(cache_directory);
    }

    download_thread = std::jthread([this, user_agent](std::stop_token st) {
        while(!st.stop_requested()) {
            std::tuple<int, int, int> coordinates;
            try {
                coordinates = download_queue.pop(std::chrono::milliseconds(250));
            } catch(...) {
                continue;
            }

            const std::string url = generate_url(std::get<0>(coordinates), std::get<1>(coordinates),
                                                 std::get<2>(coordinates));

            const std::string filename = get_filename(coordinates);
            const std::filesystem::path filepath = this->cache_directory / filename;
            const std::filesystem::path filepath_tmp = this->cache_directory / (filename + ".tmp");

            std::ofstream file(filepath_tmp, std::ios::binary);

            CURL *curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
            curl_easy_setopt(
                curl, CURLOPT_WRITEFUNCTION,
                +[](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
                    std::ostream *stream = static_cast<std::ostream *>(userdata);
                    stream->write(ptr, size * nmemb);
                    return size * nmemb;
                });
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
            if(curl_easy_perform(curl) == CURLE_OK) {
                std::filesystem::rename(filepath_tmp, filepath);
            }
            curl_easy_cleanup(curl);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    });
}

void server::draw(ImDrawList &drawer,
                  const ImVec2 position,
                  const int zoom,
                  const int x,
                  const int y) {
    const int tiles = 1 << zoom;

    if((x < 0) || (x >= tiles) || (y < 0) || (y >= tiles)) {
        return;
    }

    const std::tuple<int, int, int> coordinates(zoom, x, y);

    const std::filesystem::path filepath = cache_directory / get_filename(coordinates);

    if(!std::filesystem::exists(filepath)) {
        if(!download_queue.contains(coordinates)) {
            download_queue.push(coordinates);
        }

        drawer.AddRect(position, ImVec2(position.x + 256, position.y + 256),
                       IM_COL32(255, 255, 255, 255), 0, ImDrawFlags_None, 1);
        char buf[64];
        sprintf(buf, "(%d, %d)", y, x);
        drawer.AddText(position, IM_COL32(255, 255, 255, 255), buf);

        return;
    }

    if(!textures.contains(coordinates)) {
        int width;
        int height;
        int channels;
        const unsigned char *pixels = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     pixels);

        textures.insert({coordinates, texture});
    }

    drawer.AddImage(static_cast<ImTextureID>(textures.at(coordinates)), position,
                    ImVec2(position.x + 256, position.y + 256));
}
