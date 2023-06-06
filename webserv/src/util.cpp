#include "../header/Http.hpp"

void	Http::printConfigInfo() {
	try {
		for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator itt = this->server_block.begin(); itt != this->server_block.end(); itt++) {
			ServerBlock server = itt->second;
			std::cout << "server {" << std::endl;
			std::cout << "    listen " << server.port << std::endl;
			if (!server.server_name.empty())
				std::cout << "    server_name " << server.server_name << std::endl;
			if (!server.root.empty())
				std::cout << "    root " << server.root << std::endl;
			if (server.client_body_size)
				std::cout << "    client_body_size " << server.client_body_size << std::endl;
			if (!server.index.empty())
				std::cout << "    idnex " << server.index << std::endl;
			if (!server.error_page.empty())
				std::cout << "    error_page " << server.error_page << std::endl;
			if (!server.index_root.empty())
				std::cout << "    index_root " << server.index_root << std::endl;
			std::cout << "    host " << server.host << std::endl;
			for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++) {
				LocationBlock location = it->second;
				
				std::cout << std::endl;
				std::cout << "    location " + location.default_root + " {" << std::endl;
				std::cout << "        allow_methods ";
				for (int i = 0; i < 3; i++) {
					if (location.methods[i]) {
						if (i == GET)
							std::cout << "GET ";
						else if (i == POST)
							std::cout << "POST ";
						else if (i == DELETE)
							std::cout << "DELETE ";
					}
				}
				std::cout << std::endl;
				if (location.autoindex)
					std::cout << "        autoindex on" << std::endl;
				if (!location.root.empty()) 
					std::cout << "        root " << location.root << std::endl;
				if (!location.index.empty())
					std::cout << "        index " << location.index << std::endl;
				if (location.ret)
					std::cout << "        return " << location.redirect << std::endl;
				if (!location.cgi_bin.empty())
					std::cout << "        cgi-bin " << location.cgi_bin << std::endl;
				std::cout << "        index_root " << location.index_root << std::endl;
				std::cout << "    }" << std::endl;
			}
			std::cout << "}\n" << std::endl;
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

int Http::ft_stoi(const std::string &str) {
    int ret = 0;

    for (int i = 0; str[i]; i++) {
        if (!('0' <= str[i] && str[i] <= '9') || ret < 0)
			occurException(str, CONFIG);
		ret *= 10;
		ret += str[i] - '0';
    }
	return (ret);
}

std::string Http::ft_inet_ntoa(uint32_t ipaddr) {
    uint32_t netaddr = htonl(ipaddr);

    std::ostringstream oss;
    oss << ((netaddr >> 24) & 0xFF) << '.'
        << ((netaddr >> 16) & 0xFF) << '.'
        << ((netaddr >> 8) & 0xFF) << '.'
        << (netaddr & 0xFF);

    return oss.str();
}

uint16_t    Http::ft_ntohs(uint16_t port) { return (((port & 0xFF00) >> 8) | ((port & 0x00FF) << 8)); }