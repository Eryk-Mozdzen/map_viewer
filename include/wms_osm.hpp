#ifndef WMS_OSM_HPP
#define WMS_OSM_HPP

#include <string>

#include "wms.hpp"

class wms_osm : public wms {
    std::string generate_url(const int zoom, const int x, const int y);

public:
    wms_osm();
};

#endif
