#include "../header/Http.hpp"

Http::Http() {}
Http::Http(const std::string &path)
{
	ParsingConfig(path);
	checkValidConfig();
	checkExistFile();
	SettingHttp();
	printConfigInfo();
	runServer();
}
Http::~Http() {}

void    Http::occurException(const std::string &msg, exception type)
{
	std::cerr << msg << std::endl;

	if (type == CONFIG)
		throw NotValidConfigFileException();
	else if (type == FILEROOT)
		throw NoSuchFileException();
	else if (type == EMPTY)
		throw EmptyFileException();
	else if (type == PORT)
		throw ServerPortOverlapException();
	else if (type == ROOT)
		throw LocationRootOverlapException();
	else if (type == ADDR)
		throw notValidAddrException();
	else if (type == HTTP)
		throw SettingHttpException();
	else if (type == SERVER)
		throw RunServerException();
}

void	Http::checkValidAddr(const std::string &host)
{
	size_t	len;
	size_t	temp = 0;
	int		cnt = 0;

	for (; (len = host.find(".", temp)) != std::string::npos;)
	{
		cnt++;
		if (ft_stoi(host.substr(temp, len - temp)) > 255)
			occurException(host, ADDR);
		temp = len + 1;
	}
	if (ft_stoi(host.substr(temp)) > 255 || cnt != 3)
		occurException(host, ADDR);
}

void    Http::checkOverlapLocationRoot(const std::string &root, ServerBlock &server)
{
	int	cnt = 0;

	for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++)
	{
		if (!it->first.compare(root))
			cnt++;
		if (cnt >= 2)
			occurException(root, ROOT);
	}
}

void    Http::checkExistFile()
{
	std::ifstream	tmp;

	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
	{
		ServerBlock	&server = it->second;
		if (!server.root.empty())
		{
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
		for (std::vector<std::pair<std::string, LocationBlock> >::iterator itt = server.location_block.begin(); itt != server.location_block.end(); itt++)
		{
			LocationBlock	&location = itt->second;

			if (!location.root.empty())
			{
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

	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
	{
		if (it->first == port)
			cnt++;
		if (cnt >= 2)
		{
			std::stringstream a(port);
			std::string tmp;
			a >> tmp;
			occurException(tmp, PORT);
		}
	}
}

void	Http::checkValidConfig()
{
	std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin();
	
	for (; it != this->server_block.end(); it++)
	{
		checkOverlapServerPort(it->first);
		checkValidAddr(it->second.host);
		for (std::vector<std::pair<std::string, LocationBlock> >::iterator itt = it->second.location_block.begin(); itt != it->second.location_block.end(); itt++)
			checkOverlapLocationRoot(itt->first, it->second);
	}
}

void	Http::server_block_argu_split(std::stringstream &ss, s_block_type t, ServerBlock &ret)
{
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

void	Http::location_block_argu_split(std::stringstream &ss, l_block_type t, LocationBlock &ret)
{
	std::string tmp, temp;

	if (t == METHOD)
	{
		for (int i = 0; i < 3; i++)
			ret.methods[i] = false;
		while (1)
		{
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
	}
	else
	{
		ss >> tmp >> temp;
		if (temp.length())
			occurException(tmp + temp, CONFIG);
		if (t == AUTOINDEX)
		{
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
		else if (t == RETURN)
		{
			ret.ret = true;
			ret.redirect = tmp;
		}
		else if (t == CGI)
			ret.cgi_bin = tmp;
	}
}

std::pair<std::string, LocationBlock>	Http::location_block_split(std::ifstream &config, std::string &default_root)
{
	LocationBlock	ret;
	std::string line, cmd, temp;
	int			cnt[6] = {};

	for (int i = 0; i < 3; i++)
		ret.methods[i] = true;
	ret.default_root = default_root;
	while (1)
	{
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
		if (!cmd.compare("}"))
		{
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
	for (int i = 0; i < 6; i++)
	{
		if (cnt[i] >= 2)
		{
			std::cerr << "overlap location block option" << std::endl;
			throw	NotValidConfigFileException();
		}
	}
	return (std::make_pair(ret.default_root, ret));
}

std::pair<unsigned short, ServerBlock>	Http::Server_split(std::ifstream &config)
{
	ServerBlock	ret;
	std::string	line, cmd, temp, tmp, tt;
	int			CloseBraceCnt = 0;
	int			cnt[7] = {};

	ret.client_body_size = 0;
	ret.port = 80;
	while (1)
	{
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
		if (!cmd.compare("}") && ++CloseBraceCnt)
		{
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
		else if (!cmd.compare("location"))
		{
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

void    Http::ParsingConfig(const std::string &path)
{
	std::ifstream	config;
	std::string		line, cmd, temp, tmp;

	config.open(path.c_str());
	if (!config.is_open())
		throw	NoSuchFileException();
	while (1)
	{
		line.clear();
		getline(config, line);
		if (config.eof())
			break ;
		if (!line.length())
			continue ;
		std::stringstream ss(line);
		ss >> cmd;
		if (!cmd.compare("server"))
		{
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

void	Http::printConfigInfo()
{
	try
	{
		for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator itt = this->server_block.begin(); itt != this->server_block.end(); itt++)
		{
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
			for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++)
			{
				LocationBlock location = it->second;
				
				std::cout << std::endl;
				std::cout << "    location " + location.default_root + " {" << std::endl;
				std::cout << "        allow_methods ";
				for (int i = 0; i < 3; i++)
				{
					if (location.methods[i])
					{
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
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

int Http::ft_stoi(const std::string &str)
{
    int ret = 0;

    for (int i = 0; str[i]; i++)
    {
        if (!('0' <= str[i] && str[i] <= '9') || ret < 0)
			occurException(str, CONFIG);
		ret *= 10;
		ret += str[i] - '0';
    }
	return (ret);
}

void    Http::SettingHttp()
{
	if ((this->kq = kqueue()) == -1)
		occurException("kqueue()", HTTP);
	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
	{
		ServerBlock &server = it->second;

		if ((server.serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			occurException("socket()", HTTP);
		std::memset(&server.serv_adr, 0, sizeof(server.serv_adr));
		server.serv_adr.sin_family = AF_INET;
		server.serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
		server.serv_adr.sin_port = htons(server.port);
		if (bind(server.serv_sock, (struct sockaddr *)&server.serv_adr, sizeof(server.serv_adr)))
			occurException("bind()", HTTP);
		if (listen(server.serv_sock, LISTEN_SIZE))
			occurException("listen()", HTTP);
		EV_SET(&server.chagelist, server.serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
			occurException("kevent()", HTTP);
	}
}

void    Http::runServer()
{
	int	clnt_sock, nevents, sockfd;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_size;
	struct kevent evlist[MAX_EVENTS];

	while (true)
	{
		if ((nevents = kevent(this->kq, NULL, 0, evlist, MAX_EVENTS, NULL)) == -1)
			occurException("kevent()", SERVER);
		for (int i = 0; i < nevents; i++)
		{
			sockfd = evlist[i].ident;
			for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
			{
				ServerBlock &server = it->second;
				if (sockfd == server.serv_sock)
				{
					clnt_adr_size = sizeof(clnt_adr);
					if ((clnt_sock = accept(sockfd, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1)
						occurException("accept()", SERVER);
					std::cout << "Connection Request : " << ft_inet_ntoa(clnt_adr.sin_addr.s_addr) << " : " << ft_ntohs(clnt_adr.sin_port) << std::endl;
					EV_SET(&server.chagelist, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					EV_SET(&server.chagelist, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
						occurException("kevent()", SERVER);
				}
				else
				{
					char req_line[BUF_SIZE];
					ssize_t str_len = read(sockfd, req_line, BUF_SIZE);
					if (str_len == 0)
					{
						close(sockfd);
						continue ;
					}

					std::string method, file_name;
					std::stringstream req_msg(req_line);
					std::stringstream tmp(req_line);
					std::string line;
					while (std::getline(tmp, line))
					{
						std::cout << "line : " << line << std::endl;
					}
					req_msg >> method >> file_name;
					std::cout << "req line : " << req_line << std::endl;

					std::cout << "method : " << method << std::endl;
					std::cout << "file_name : " << file_name << std::endl;
					if (method.compare("GET"))
					{
						// send_error(sockfd)
						// close(sockfd);
						continue ;
					}
					send_data(sockfd, findRoot(server, file_name));
					close(sockfd);
				}
			}
		}
	}

	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
		close(it->second.serv_sock);
}

void Http::send_error(int clnt_sock)
{   
    char protocol[]="HTTP/1.1 400 Bad Request\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]="Content-length:2048\r\n";
    char cnt_type[]="Content-type:text/html\r\n\r\n";
    char content[]="<html><head><title>NETWORK</title></head>"
           "<body><font size=+5><br>오류 발생! 요청 파일명 및 요청 방식 확인!"
           "</font></body></html>";

    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, cnt_type, strlen(cnt_type));
    write(clnt_sock, content, strlen(content));
}

std::string	Http::findRoot(ServerBlock &server, std::string file_name)
{
	for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++)
	{
		std::cout << "location root : " << it->second.default_root << ", file_name : " << file_name << std::endl;
		if (!it->second.default_root.compare(file_name))
			return (it->second.index_root);
	}
	return ("");
}

void    Http::send_data(int clnt_sock, std::string file_name)
{
	char protocol[]="HTTP/1.0 200 OK\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]="Content-length:2048\r\n";
	const char *type;
	std::string ct = "text/html";
	
	std::string tmp;
	std::string cnt_type = "Content-type:" + ct + "\r\n\r\n";
	std::ifstream file;

	std::cout << "파일 네임 : " + file_name << std::endl;
	file.open(file_name.c_str());
    if(!file.is_open())
    {
		std::cout << "에러 발생 !!!!!!!!!!!!!! " << std::endl;
        send_error(clnt_sock);
        return;
    }
	type = cnt_type.c_str();

    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, type, std::strlen(type));
	while (std::getline(file, tmp))
	{
		tmp += '\n';
		const char *msg = tmp.c_str();
		write(clnt_sock, msg, std::strlen(msg));
	}
	file.close();
}

std::string Http::ft_inet_ntoa(uint32_t ipaddr)
{
    uint32_t netaddr = htonl(ipaddr);

    std::ostringstream oss;
    oss << ((netaddr >> 24) & 0xFF) << '.'
        << ((netaddr >> 16) & 0xFF) << '.'
        << ((netaddr >> 8) & 0xFF) << '.'
        << (netaddr & 0xFF);

    return oss.str();
}

uint16_t    Http::ft_ntohs(uint16_t port)
{
	return (((port & 0xFF00) >> 8) | ((port & 0x00FF) << 8));
}

const char *Http::NotValidConfigFileException::what() const throw()
{
    return ("Error : Not Valid Configuration File");
}

const char *Http::EmptyFileException::what() const throw()
{
	return ("Error : Empty File");
}

const char *Http::ServerPortOverlapException::what() const throw()
{
	return ("Error : Server Port Overlap");
}

const char *Http::LocationRootOverlapException::what() const throw()
{
	return ("Error : Location Block Default Root Overlap");
}

const char *Http::NoSuchFileException::what() const throw()
{
	return ("Error : No Such File");
}

const char *Http::notValidAddrException::what() const throw()
{
	return ("Error : Is Not Valid Address");
}

const char *Http::SettingHttpException::what() const throw()
{
	return ("Error : Fail Server Setting function");
}

const char *Http::RunServerException::what() const throw()
{
	return ("Error : Occured Exception During Run Server");
}
