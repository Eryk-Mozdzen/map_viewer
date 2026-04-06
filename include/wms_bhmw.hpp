#ifndef WMS_BHMW_HPP
#define WMS_BHMW_HPP

#include <atomic>
#include <string>

#include "wms.hpp"

class wms_bhmw : public wms {
    std::atomic_bool enable_transparent = true;

    std::string generate_url(const int tile_zoom, const int tile_x, const int tile_y);

public:
    wms_bhmw();

    void transparent(const bool enable);
};

#endif
