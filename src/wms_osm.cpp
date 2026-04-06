#include <string>

#include "wms_osm.hpp"

wms_osm::wms_osm() : wms{"cache_osm"} {
}

std::string wms_osm::generate_url(const int tile_zoom, const int tile_x, const int tile_y) {
    return "https://tile.openstreetmap.org/" + std::to_string(tile_zoom) + "/" +
           std::to_string(tile_x) + "/" + std::to_string(tile_y) + ".png";
}
