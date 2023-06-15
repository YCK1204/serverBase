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
			break ;
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

void	Http::addFdSet(int clnt_sock, fd_set &event) {
	FD_SET(clnt_sock, &event);
	if (clnt_sock > fd_max)
		fd_max = clnt_sock;
}

void	Http::clearFdSet(int clnt_sock, fd_set &event) {
	FD_CLR(clnt_sock, &event);
	if (clnt_sock == fd_max)
		fd_max--;
	close(clnt_sock);
}

std::string    Http::spaceTrim(std::string str) {
    int start, end;
    for (start = 0; str[start]; start++) {
        if (str[start] != ' ')
            break ;
    }
    for (end = str.length() - 1; end > 0; end--) {
        if (str[end] != ' ')
            break ;
    }
    return str.substr(start, end - start + 1);
}

bool	Http::checkValidateAddress(int addr[4]) {
	int validAddr[3][4], err = 1;

	for (int i = 0; i < 4; i++) {
		if (addr[i] > 255 || addr[i] < 0) {
			return true ;
		}
	}
	validAddr[0][0] = 0;
	validAddr[0][1] = 0;
	validAddr[0][2] = 0;
	validAddr[0][3] = 0;

	validAddr[1][0] = 127;
	validAddr[1][1] = 0;
	validAddr[1][2] = 0;
	validAddr[1][3] = 1;

	validAddr[2][0] = 10;
	validAddr[2][1] = 31;
	validAddr[2][2] = 1;
	validAddr[2][3] = 4;
	for (int i = 0; i < 3; i++)
	{
		if (addr[0] == validAddr[i][0] && addr[1] == validAddr[i][1] && \
			addr[2] == validAddr[i][2] && addr[3] == validAddr[i][3]
		)
			err = 0;
	}
	if (err) {
		return true ;
	}
	return false;
}

ServerBlock Http::getServer(std::string host, std::string root) {
    int     port = ft_stoi(host);
    bool    f = false;
    std::vector<ServerBlock>::iterator it;

    for (it = this->server.begin(); it != this->server.end(); it++) {
        ServerBlock &server = *it;
        if (server.port == port) { // 포트가 같을 때
            for (std::vector<LocationBlock>::iterator it = server.location.begin(); it != server.location.end(); it++) {
                LocationBlock &location = *it;
                if (!location.default_root.compare(root)) { // 루트 발견
                    f = true;
                    break ;
                }
            }
        }
        if (f)
            break ;
    }
    if (it == this->server.end()) { // 루트가 없을 때
        for (it = this->server.begin(); it != this->server.end(); it++) {
            ServerBlock &server = *it;
            if (server.port == port)
                break ;
        }
    }

    if (it == this->server.end())  // 포트가 이상할 때
        return *(this->server.begin());
    return *it;
}

LocationBlock    Http::getLocation(std::string root, ServerBlock server) {
    std::vector<LocationBlock>::iterator it;

    for (it = server.location.begin(); it != server.location.end(); it++) {
        LocationBlock   &location = *it;
        if (!location.default_root.compare(root))
            break ;
    }

    if (it == server.location.end())
        return *(server.location.begin());
    return *it;
}

std::string Http::getIndexRoot(ServerBlock server, LocationBlock location, int clnt_sock) {
    std::string ret;

    if (location.root.empty())
        ret += server.root;
    else
        ret += location.root;
    if (location.index.empty())
        ret += server.index;
    else
        ret += location.index;
    return ret;
}

bool compareFiles(const FileInfo& file1, const FileInfo& file2) {
    return file1.name < file2.name;
}

std::string Http::formatSize(double size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }

    char buffer[100];
    std::sprintf(buffer, "%.2f %s", size, units[unitIndex]);
    return std::string(buffer);
}

std::string Http::formatTime(const time_t& time) {
    struct tm* timeinfo;
    char buffer[80];
    timeinfo = localtime(&time);
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", timeinfo);
    return std::string(buffer);
}

std::string Http::buildAutoindex(std::string dir_root) {
    std::string ret, msg;
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    std::vector<FileInfo> files;

    dir = opendir(dir_root.c_str());
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            std::string file_path = dir_root + "/" + entry->d_name;
            if (stat(file_path.c_str(), &file_stat) == -1) {
                err = 503;
                return "";
            }
            FileInfo file;
            file.name = static_cast<std::string>(entry->d_name);
            file.lastModified = file_stat.st_mtime;
            file.size = file_stat.st_size;
			if (S_ISDIR(file_stat.st_mode))
				file.is_dir = true;
			else
				file.is_dir = false;
            files.push_back(file);
        }
        std::sort(files.begin(), files.end(), compareFiles);

        msg += "    <table>\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";
        for (std::vector<FileInfo>::iterator it = files.begin(); it != files.end(); it++) {
            msg += "    <tr>";
			if (it->is_dir)
            	msg += "        <td><a href=\"" + dir_root + "/" + it->name + "/\">" + it->name + "/</a></td>\n";
			else
            	msg += "        <td><a href=\"" + dir_root + "/" + it->name + "\">" + it->name + "</a></td>\n";
            msg += "        <td>" + formatTime(it->lastModified) + "</td>\n";
            double fileSize = static_cast<double>(it->size);
            msg += "        <td>" + formatSize(fileSize) + "</td>\n";
            msg += "    </tr>\n";
        }
        msg += "    </table>\n";
        ret = buildHtml(msg);
    } else {
        err = 403;
        return "";
    }
    return ret;
}