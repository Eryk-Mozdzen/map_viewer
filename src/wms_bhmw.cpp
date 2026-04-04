#include <chrono>
#include <cmath>
#include <sstream>
#include <string>
#include <tuple>

#include "wms_bhmw.hpp"

static std::pair<double, double> num_to_deg(const int zoom, const int x, const int y) {
    const double n = std::pow(2.0, zoom);
    const double lon = x / n * 360.0 - 180.0;
    const double lat_rad = std::atan(std::sinh(M_PI * (1 - 2.0 * y / n)));
    const double lat = lat_rad * 180.0 / M_PI;
    return {lat, lon};
}

static std::pair<double, double> latlon_to_mercator(const double lat, const double lon) {
    const double x = lon * 20037508.34 / 180.0;
    const double y =
        (20037508.34 / 180.0) * std::log(std::tan((90.0 + lat) * M_PI / 360.0)) / (M_PI / 180.0);
    return {x, y};
}

wms_bhmw::wms_bhmw()
    : wms{"cache_bhmw", std::chrono::milliseconds(1000),
          "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 "
          "Safari/537.36"} {
}

std::string wms_bhmw::generate_url(const int zoom, const int x, const int y) {
    const auto [lat1, lon1] = num_to_deg(zoom, x + 0, y + 0);
    const auto [lat2, lon2] = num_to_deg(zoom, x + 1, y + 1);

    const auto [minx, maxy] = latlon_to_mercator(lat1, lon1);
    const auto [maxx, miny] = latlon_to_mercator(lat2, lon2);

    std::stringstream ss;
    ss << "https://bhmw-wms.wp.mil.pl/wms/";
    ss << "?SERVICE=WMS";
    ss << "&VERSION=1.3.0";
    ss << "&REQUEST=GetMap";
    ss << "&FORMAT=image/png";
    ss << "&TRANSPARENT=true";
    ss << "&LAYERS=ENC";
    ss << "&CSBOOL=2";
    ss << "&CSVALUE=8,,,8,,2,1";
    ss << "&OBJECTFILTERNEGATION=true";
    ss << "&OBJECT=LNDARE,M_COVR,BUAARE";
    ss << "&CRS=EPSG:3857";
    ss << "&STYLES=";
    ss << "&WIDTH=256";
    ss << "&HEIGHT=256";
    ss << "&BBOX=" << minx << "," << miny << "," << maxx << "," << maxy;

    return ss.str();
}
