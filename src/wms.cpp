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

#include "wms.hpp"

static std::string get_filename(const std::tuple<int, int, int> coordinates) {
    return std::to_string(std::get<0>(coordinates)) + "_" +
           std::to_string(std::get<1>(coordinates)) + "_" +
           std::to_string(std::get<2>(coordinates)) + ".png";
}

wms::wms(const std::filesystem::path cache_directory,
         const std::chrono::milliseconds minimal_request_period,
         const std::string user_agent)
    : cache_directory{cache_directory}, cache_clear{false} {
    download_thread = std::jthread([this, minimal_request_period, user_agent](std::stop_token st) {
        std::chrono::steady_clock::time_point downland_timepoint = std::chrono::steady_clock::now();

        while(!st.stop_requested()) {
            if(cache_clear) {
                cache_clear = false;

                download_queue.clear();

                for(const auto &[coords, texture] : textures) {
                    glDeleteTextures(1, &texture);
                }
                textures.clear();

                std::filesystem::remove_all(this->cache_directory);
            }

            if(!std::filesystem::exists(this->cache_directory)) {
                std::filesystem::create_directories(this->cache_directory);
            }

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
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            std::this_thread::sleep_until(downland_timepoint + minimal_request_period);
            if(curl_easy_perform(curl) == CURLE_OK) {
                std::filesystem::rename(filepath_tmp, filepath);
            }
            downland_timepoint = std::chrono::steady_clock::now();

            curl_easy_cleanup(curl);
        }
    });
}

wms::~wms() {
    for(const auto &[coords, texture] : textures) {
        glDeleteTextures(1, &texture);
    }
}

void wms::draw(ImDrawList *drawer,
               const int screen_x,
               const int screen_y,
               const int tile_zoom,
               const int tile_x,
               const int tile_y) {
    const int tiles = 1 << tile_zoom;

    if((tile_x < 0) || (tile_x >= tiles) || (tile_y < 0) || (tile_y >= tiles)) {
        return;
    }

    const std::tuple<int, int, int> coordinates(tile_zoom, tile_x, tile_y);

    const std::filesystem::path filepath = cache_directory / get_filename(coordinates);

    if(!std::filesystem::exists(filepath)) {
        if(!download_queue.contains(coordinates)) {
            download_queue.push(coordinates);
        }
        return;
    }

    if(!textures.contains(coordinates)) {
        int width;
        int height;
        int channels;
        unsigned char *pixels = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

        if((width == tile_size) && (height == tile_size)) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         pixels);

            textures.insert({coordinates, texture});
        }

        stbi_image_free(pixels);
    }

    if(textures.contains(coordinates)) {
        drawer->AddImage(textures.at(coordinates), ImVec2(screen_x, screen_y),
                         ImVec2(screen_x + tile_size, screen_y + tile_size));
    }
}

void wms::clear_cache() {
    cache_clear = true;
}
