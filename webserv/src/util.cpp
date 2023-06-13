#include "../header/Http.hpp"

void	Http::exception_util(const std::string &str, s_block_type type) {
	if (type == HOST) {
		occurException(13, str, CONFIG, UTIL,
		"is not valid port");
	} else {
		occurException(13, str ,CONFIG, UTIL,
		"is not valie client body size");
	}
}

int Http::ft_stoi(const std::string &str, s_block_type type) {
    int ret = 0;

	if (str[0] == '-') {
		exception_util(str, type);
	}
    for (int i = 0; str[i]; i++) {
		if (str[i] == '.')
			continue ;
        if (!('0' <= str[i] && str[i] <= '9') || ret < 0) {
			exception_util(str, type);
		}
		ret *= 10;
		ret += str[i] - '0';
    }
	if (ret < 0) {
		exception_util(str, type);
	}
	return (ret);
}

int Http::ft_stoi(const std::string &str) {
    int ret = 0;

	if (str[0] == '-') {
		return 0;
	}
    for (int i = 0; str[i]; i++) {
		if (str[i] == '.')
			continue ;
        if (!('0' <= str[i] && str[i] <= '9') || ret < 0) {
			return 0;
		}
		ret *= 10;
		ret += str[i] - '0';
    }
	return (ret);
}

void	Http::printConfigInfo() {
	for (std::vector<ServerBlock>::iterator itt = this->server.begin(); itt != this->server.end(); itt++) {
		ServerBlock server = *itt;
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
		for (std::vector<LocationBlock>::iterator it = server.location.begin(); it != server.location.end(); it++) {
			LocationBlock location = *it;
			
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
			if (!location.cgi.empty())
				std::cout << "        cgi-bin " << location.cgi << std::endl;
			std::cout << "    }" << std::endl;
		}
		std::cout << "}\n" << std::endl;
	}
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

bool	Http::ExistFile(std::string &root) {
	std::ifstream file;

	file.open(root);
	return (file.is_open());
}

bool	Http::ExistDirectory(std::string &root) {
	return (opendir(root.c_str()) != NULL);
}

void    Http::recursive_to_string(int n, std::string &ret) {
    if (n >= 10) {
		recursive_to_string(n / 10, ret);
		ret += (n % 10) + '0';
	}
    else
	    ret += n + '0';
}

std::string Http::ft_to_string(int n) {
	std::string ret;

	if (n == -2147483648)
		return "-2147483648";
	if (n < 0) {
		n *= -1;
		ret += '-';
	}
	recursive_to_string(n, ret);
    return ret;
}

std::string Http::getDate() {
    int month;
    char buffer[80];
    ssize_t monthPos;
    struct tm* timeInfo;
    struct tm* selTimeInfo;
    time_t rawTime, selTime;
    std::string dateHeader, monthName;
    const char* monthNames[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    std::time(&rawTime);
    timeInfo = std::gmtime(&rawTime);
    selTime = rawTime + (9 * 3600);
    selTimeInfo = std::gmtime(&selTime);
    std::strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S SEL", selTimeInfo);
    dateHeader = static_cast<std::string>(buffer);
    month = selTimeInfo->tm_mon;
    monthName = monthNames[month];
    monthPos = dateHeader.find("MMM");
    if (monthPos != std::string::npos)
        dateHeader.replace(monthPos, 3, monthName);
    return dateHeader;
}