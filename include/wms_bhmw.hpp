#ifndef WMS_BHMW_HPP
#define WMS_BHMW_HPP

#include <atomic>
#include <string>

#include "wms.hpp"

class wms_bhmw : public wms {
public:
    enum display_category {
        BASE = 1,
        STD = 2,
        ALL = 3,
    };

    enum color_scheme {
        DAY = 1,
        DUSK = 2,
        NIGHT = 3,
    };

private:
    std::atomic_bool enable_transparent = true;
    std::atomic_bool enable_lights = true;
    std::atomic_bool enable_text = true;
    std::atomic_bool enable_soundings = true;
    std::atomic_bool enable_gray_shades = false;
    std::atomic_bool enable_chart_boundaries = true;

    std::atomic<int> safety_contour = 8; // [0; 20]
    std::atomic<int> safety_depth = 8;   // [0; 20]
    std::atomic<display_category> display = display_category::ALL;
    std::atomic<color_scheme> color = color_scheme::DAY;

    std::string generate_url(const int tile_zoom, const int tile_x, const int tile_y);

public:
    wms_bhmw();

    void transparent(const bool enable);
};

#endif
