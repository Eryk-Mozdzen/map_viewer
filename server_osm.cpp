#include <string>

#include "server_osm.hpp"

server_osm::server_osm(const std::filesystem::path cache_directory) : server{cache_directory} {
}

std::string server_osm::generate_url(const int zoom, const int x, const int y) {
    return "https://tile.openstreetmap.org/" + std::to_string(zoom) + "/" + std::to_string(x) +
           "/" + std::to_string(y) + ".png";
}
