#ifndef SERVER_OSM_HPP
#define SERVER_OSM_HPP

#include <string>

#include "server.hpp"

class server_osm : public server {
    std::string generate_url(const int zoom, const int x, const int y);

public:
    server_osm();
};

#endif
