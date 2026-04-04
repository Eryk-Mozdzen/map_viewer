#ifndef WMS_BHMW_HPP
#define WMS_BHMW_HPP

#include <string>

#include "wms.hpp"

class wms_bhmw : public wms {
    std::string generate_url(const int zoom, const int x, const int y);

public:
    wms_bhmw();
};

#endif
