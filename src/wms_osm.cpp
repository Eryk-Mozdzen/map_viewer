#include <string>

#include "wms_osm.hpp"

wms_osm::wms_osm() : wms{"cache_osm"} {
}

std::string wms_osm::generate_url(const int zoom, const int x, const int y) {
    return "https://tile.openstreetmap.org/" + std::to_string(zoom) + "/" + std::to_string(x) +
           "/" + std::to_string(y) + ".png";
}
