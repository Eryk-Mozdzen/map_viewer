#ifndef WMS_OSM_HPP
#define WMS_OSM_HPP

#include <string>

#include "wms.hpp"

class wms_osm : public wms {
    std::string generate_url(const int tile_zoom, const int tile_x, const int tile_y);

public:
    wms_osm();
};

#endif
