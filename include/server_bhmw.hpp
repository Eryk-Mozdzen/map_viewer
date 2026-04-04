#ifndef SERVER_BHMW_HPP
#define SERVER_BHMW_HPP

#include <string>

#include "server.hpp"

class server_bhmw : public server {
    std::string generate_url(const int zoom, const int x, const int y);

public:
    server_bhmw(const std::filesystem::path cache_directory);
};

#endif
