#include "../header/Http.hpp"

void	Http::checkEmptyFile(const std::string &path) {
	std::ifstream	file;
	std::string		line;

	file.open(path);
	if (file.is_open()) 
	{
		while (std::getline(file, line))
		{
			if (line.empty())
				continue ;
			else if (!line.compare("server {"))
				break ;
			else
				occurException(16, line, CONFIG, PARSING,
				"server is not started \"server {\"");
		}
	} else {
		occurException(20, path, FILEROOT, PARSING,
		"file not found");
	}
	if (line.empty())
		occurException(24, path, EMPTY, PARSING,
		"file is empty");
	file.close();
}

void    Http::ParsingConfig(const std::string &path) {
	std::ifstream	file;
	std::string		line, cmd, t;

	checkEmptyFile(path);
	file.open(path);
	while (std::getline(file, line)) {
		if (line.empty())
			continue ;
		std::stringstream ss(line);
		ss >> cmd;
		if (!cmd.compare("server")) {
			ss >> cmd;
			if (cmd.compare("{")) {
				occurException(43, line, CONFIG, PARSING,
				"server is not started \"server {\"");
			} else {
				ss >> t;
				if (!t.empty()) {
					occurException(48, line, CONFIG, PARSING,
					"server is not started \"server {\"");
				}
				this->server.push_back(makeServer(file));
			}
		} else {
			occurException(54, line, CONFIG, PARSING,
			"server is not started \"server {\"");
		}
	}
	file.close();
}

void	Http::checkValidAddr(const std::string &host) {
	size_t	len;
	size_t	temp = 0;
	int		cnt = 0, t, addr[4];

	for (; (len = host.find(".", temp)) != std::string::npos;) {
		cnt++;
		t = ft_stoi(host.substr(temp, len - temp), HOST);
		if (cnt > 3) {
			occurException(62, host, CONFIG, PARSING,
			"is not valid server address (xxx.xxx.xxx.xxx)");
		}
		addr[cnt - 1] = t;
		temp = len + 1;
	}
	t = ft_stoi(host.substr(temp, len - temp), HOST);
	addr[cnt] = t;
	if (cnt != 3) {
		occurException(62, host, CONFIG, PARSING,
		"is not valid server address (xxx.xxx.xxx.xxx)");
	}
	checkValidateAddress(addr, host);
}

void	Http::checkValidateAddress(int addr[4], const std::string &host) {
	int validAddr[3][4], err = 1;

	for (int i = 0; i < 4; i++) {
		if (addr[i] > 255) {
			occurException(86, host, CONFIG, PARSING,
			"is not valid server address (one octet 0 ~ 255)");
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
	validAddr[2][2] = 5;
	validAddr[2][3] = 2;
	for (int i = 0; i < 3; i++)
	{
		if (addr[0] == validAddr[i][0] && addr[1] == validAddr[i][1] && \
			addr[2] == validAddr[i][2] && addr[3] == validAddr[i][3]
		)
			err = 0;
	}
	if (err) {
		occurException(86, host, CONFIG, PARSING,
		"can not use server address");
	}
}

void	Http::checkValidConfig() {
	for (std::vector<ServerBlock>::iterator it = server.begin(); it != server.end(); it++) {
		ServerBlock &server = *it;
		if (server.port > 65535) {
			occurException(125, "listen", CONFIG, PARSING,
			"can not use server port");
		}
		checkValidAddr(it->host);
		for (std::vector<LocationBlock>::iterator itt = it->location.begin(); itt != it->location.end(); itt++) {
			for (std::vector<LocationBlock>::iterator ittt = it->location.begin(); ittt != it->location.end(); ittt++) {
				if (itt == ittt)
					continue ;
				if (!itt->default_root.compare(ittt->default_root)) {
					occurException(134, itt->default_root, CONFIG, PARSING,
					"overlap location block");
				}
			}
		}
	}
}