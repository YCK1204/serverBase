#include "../header/Http.hpp"

void	Http::checkValidAddr(const std::string &host) {
	size_t	len;
	size_t	temp = 0;
	int		cnt = 0;

	for (; (len = host.find(".", temp)) != std::string::npos;) {
		cnt++;
		if (ft_stoi(host.substr(temp, len - temp)) > 255)
			occurException(host, ADDR);
		temp = len + 1;
	}
	if (ft_stoi(host.substr(temp)) > 255 || cnt != 3)
		occurException(host, ADDR);
}

void    Http::checkOverlapLocationRoot(const std::string &root, ServerBlock &server) {
	int	cnt = 0;

	for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++) {
		if (!it->first.compare(root))
			cnt++;
		if (cnt >= 2)
			occurException(root, ROOT);
	}
}

void    Http::checkExistFile()
{
	std::ifstream	tmp;

	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++) {
		ServerBlock	&server = it->second;
		if (!server.root.empty()) {
			if (server.index.empty())
				occurException(server.root, FILEROOT);
			tmp.open((server.root + server.index.substr(1)).c_str());
			if (!tmp.is_open())
				occurException(server.root, FILEROOT);		
			tmp.close();
			tmp.clear();
			server.index_root = (server.root + server.index.substr(1));
		}
		tmp.open((server.root + server.error_page.substr(1)).c_str());
		if (!tmp.is_open())
			occurException(server.root + server.error_page.substr(1), FILEROOT);
		tmp.close();
		tmp.clear();
		server.error_page = (server.root + server.error_page.substr(1));
		for (std::vector<std::pair<std::string, LocationBlock> >::iterator itt = server.location_block.begin(); itt != server.location_block.end(); itt++) {
			LocationBlock	&location = itt->second;
			if (!location.root.empty()) {
				if (location.index.empty())
					occurException(location.root, FILEROOT);
				tmp.open((location.root + location.index.substr(1)).c_str());
				if (!tmp.is_open())
					occurException(location.root + location.index.substr(1), FILEROOT);
				tmp.close();
				tmp.clear();
				location.index_root = (location.root + location.index.substr(1));
			}
			else
				location.index_root = server.index_root;
		}
	}
}

void    Http::checkOverlapServerPort(const unsigned short &port)
{
	int	cnt = 0;

	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++) {
		if (it->first == port)
			cnt++;
		if (cnt >= 2) {
			std::stringstream a(port);
			std::string tmp;
			a >> tmp;
			occurException(tmp, PORT);
		}
	}
}

void	Http::checkValidConfig() {
	std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin();
	
	for (; it != this->server_block.end(); it++) {
		checkOverlapServerPort(it->first);
		checkValidAddr(it->second.host);
		for (std::vector<std::pair<std::string, LocationBlock> >::iterator itt = it->second.location_block.begin(); itt != it->second.location_block.end(); itt++)
			checkOverlapLocationRoot(itt->first, it->second);
	}
}

void	Http::server_block_argu_split(std::stringstream &ss, s_block_type t, ServerBlock &ret) {
	std::string tmp, temp;
	int			val;

	ss >> tmp >> temp;
	if (t == LISTEN || t == BODY_SIZE)
		val = ft_stoi(tmp);
	if (temp.length() || (t == LISTEN && val > 65535))
		throw	NotValidConfigFileException();
	if (t == LISTEN)
		ret.port = static_cast<unsigned short>(val);
	else if (t == S_ROOT)
		ret.root = tmp;
	else if (t == SERVER_NAME)
		ret.server_name = tmp;
	else if (t == ERROR_PAGE)
		ret.error_page = tmp;
	else if (t == S_INDEX)
		ret.index = tmp;
	else if (t == BODY_SIZE)
		ret.client_body_size = val;
	else if (t == HOST)
		ret.host = tmp;
}

void	Http::location_block_argu_split(std::stringstream &ss, l_block_type t, LocationBlock &ret) {
	std::string tmp, temp;

	if (t == METHOD) {
		for (int i = 0; i < 3; i++)
			ret.methods[i] = false;
		while (1) {
			tmp.clear();
			ss >> tmp;
			if (tmp.empty())
				break ;
			if (!tmp.compare("GET"))
				ret.methods[GET] = true;
			else if (!tmp.compare("POST"))
				ret.methods[POST] = true;
			else if (!tmp.compare("DELETE"))
				ret.methods[DELETE] = true;
			else
				occurException(tmp, CONFIG);
		}
	} else {
		ss >> tmp >> temp;
		if (temp.length())
			occurException(tmp + temp, CONFIG);
		if (t == AUTOINDEX) {
			if (!tmp.compare("on"))
				ret.autoindex = true;
			else if (!tmp.compare("off"))
				ret.autoindex = false;
			else
				occurException("autoindex " + tmp, CONFIG);
		}
		else if (t == L_ROOT)
			ret.root = tmp;
		else if (t == L_INDEX)
			ret.index = tmp;
		else if (t == RETURN) {
			ret.ret = true;
			ret.redirect = tmp;
		}
		else if (t == CGI)
			ret.cgi_bin = tmp;
	}
}

std::pair<std::string, LocationBlock>	Http::location_block_split(std::ifstream &config, std::string &default_root) {
	LocationBlock	ret;
	std::string line, cmd, temp;
	int			cnt[6] = {};

	for (int i = 0; i < 3; i++)
		ret.methods[i] = true;
	ret.default_root = default_root;
	ret.ret = false;
	while (1) {
		line.clear();
		cmd.clear();
		temp.clear();
		getline(config, line);
		if (config.eof())
			throw	NotValidConfigFileException();
		if (!line.length())
			continue ;
		std::stringstream	ss(line);
		ss >> cmd;
		if (!cmd.compare("}")) {
			ss >> temp;
			if (temp.length())
				occurException(cmd + temp, CONFIG);
			break ;
		}
		else if (!cmd.compare("allow_methods") && ++cnt[METHOD])
			location_block_argu_split(ss, METHOD, ret);
		else if (!cmd.compare("autoindex") && ++cnt[AUTOINDEX])
			location_block_argu_split(ss, AUTOINDEX, ret);
		else if (!cmd.compare("root") && ++cnt[L_ROOT])
			location_block_argu_split(ss, L_ROOT, ret);
		else if (!cmd.compare("index") && ++cnt[L_INDEX])
			location_block_argu_split(ss, L_INDEX, ret);
		else if (!cmd.compare("return") && ++cnt[RETURN])
			location_block_argu_split(ss, RETURN, ret);
		else if (!cmd.compare("cgi-bin") && ++cnt[CGI])
			location_block_argu_split(ss, CGI, ret);
		else
			occurException(cmd, CONFIG);
	}
	for (int i = 0; i < 6; i++) {
		if (cnt[i] >= 2) {
			std::cerr << "overlap location block option" << std::endl;
			throw	NotValidConfigFileException();
		}
	}
	return (std::make_pair(ret.default_root, ret));
}

std::pair<unsigned short, ServerBlock>	Http::Server_split(std::ifstream &config) {
	ServerBlock	ret;
	std::string	line, cmd, temp, tmp, tt;
	int			CloseBraceCnt = 0;
	int			cnt[7] = {};

	ret.client_body_size = 0;
	ret.port = 8080;
	while (1) {
		line.clear();
		cmd.clear();
		temp.clear();
		getline(config, line);
		if (!line.length() && config.eof())
			throw	NotValidConfigFileException();
		if (!line.length())
			continue ;
		std::stringstream	ss(line);
		ss >> cmd;
		if (!cmd.compare("}") && ++CloseBraceCnt) {
			ss >> temp;
			if (temp.length())
				occurException(cmd + temp, CONFIG);
			break ;
		}
		else if (!cmd.compare("listen") && ++cnt[LISTEN])
			server_block_argu_split(ss, LISTEN, ret);
		else if (!cmd.compare("error_page") && ++cnt[ERROR_PAGE])
			server_block_argu_split(ss, ERROR_PAGE, ret);
		else if (!cmd.compare("host") && ++cnt[HOST])
			server_block_argu_split(ss, HOST, ret);
		else if (!cmd.compare("client_body_size") && ++cnt[BODY_SIZE])
			server_block_argu_split(ss, BODY_SIZE, ret);
		else if (!cmd.compare("index") && ++cnt[S_INDEX])
			server_block_argu_split(ss, S_INDEX, ret);
		else if (!cmd.compare("root") && ++cnt[S_ROOT])
			server_block_argu_split(ss, S_ROOT, ret);
		else if (!cmd.compare("server_name") && ++cnt[SERVER_NAME])
			server_block_argu_split(ss, SERVER_NAME, ret);
		else if (!cmd.compare("location")) {
			ss >> temp >> tmp >> tt;
			if (tmp.compare("{"))
				occurException(tmp, CONFIG);
			else if (tt.length())
				occurException(tt, CONFIG);
			ret.location_block.push_back(location_block_split(config, temp));
		}
		else if (!cmd.empty() && cmd[0] != '#')
			occurException(cmd, CONFIG);
	}
	for (int i = 0; i < 4; i++)
		if (cnt[i] != 1)
			throw	NotValidConfigFileException();
	if (cnt[4] >= 2 || cnt[5] >= 2 || cnt[6] >= 2 || CloseBraceCnt != 1)
		throw	NotValidConfigFileException();
	return (std::make_pair(ret.port, ret));
}

void    Http::ParsingConfig(const std::string &path) {
	std::ifstream	config;
	std::string		line, cmd, temp, tmp;

	config.open(path.c_str());
	if (!config.is_open())
		throw	NoSuchFileException();
	while (1) {
		line.clear();
		getline(config, line);
		if (config.eof())
			break ;
		if (!line.length())
			continue ;
		std::stringstream ss(line);
		ss >> cmd;
		if (!cmd.compare("server")) {
			ss >> temp >> tmp;
			if (temp.compare("{") || tmp.length())
				throw	NotValidConfigFileException();
			this->server_block.push_back(Server_split(config));
		}
		else
			throw	NotValidConfigFileException();
	}
	if (!this->server_block.size())
		throw	EmptyFileException();
}