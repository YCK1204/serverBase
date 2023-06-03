#include "Http.hpp"

Http::Http() {}
Http::Http(const std::string &path)
{
	ParsingConfig(path);
}
Http::~Http() {}


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
				throw	NotValidConfigFileException();
		}
	}
	else
	{
		ss >> tmp >> temp;
		if (temp.length())
			throw	NotValidConfigFileException();
		if (t == AUTOINDEX)
		{
			if (!tmp.compare("on"))
				ret.autoindex = true;
			else if (!tmp.compare("off"))
				ret.autoindex = false;
			else
				throw	NotValidConfigFileException();
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
				throw	NotValidConfigFileException();
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
	}
	for (int i = 0; i < 6; i++)
		if (cnt[i] >= 2)
			throw	NotValidConfigFileException();
	return (std::make_pair(ret.default_root, ret));
}

std::pair<unsigned short, ServerBlock>	Http::Server_split(std::ifstream &config)
{
	ServerBlock	ret;
	std::string	line, cmd, temp, tmp, tt;
	int			CloseBraceCnt = 0;
	int			cnt[6] = {};

	ret.client_body_size = 0;
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
				throw	NotValidConfigFileException();
			break ;
		}
		else if (!cmd.compare("listen") && ++cnt[LISTEN])
			server_block_argu_split(ss, LISTEN, ret);
		else if (!cmd.compare("root") && ++cnt[S_ROOT])
			server_block_argu_split(ss, S_ROOT, ret);
		else if (!cmd.compare("server_name") && ++cnt[SERVER_NAME])
			server_block_argu_split(ss, SERVER_NAME, ret);
		else if (!cmd.compare("error_page") && ++cnt[ERROR_PAGE])
			server_block_argu_split(ss, ERROR_PAGE, ret);
		else if (!cmd.compare("index") && ++cnt[S_INDEX])
			server_block_argu_split(ss, S_INDEX, ret);
		else if (!cmd.compare("client_body_size") && ++cnt[BODY_SIZE])
			server_block_argu_split(ss, BODY_SIZE, ret);
		else if (!cmd.compare("location"))
		{
			ss >> temp >> tmp >> tt;
			if (tmp.compare("{") || tt.length())
				throw	NotValidConfigFileException();
			ret.location_block.push_back(location_block_split(config, temp));
		}
	}
	for (int i = 0; i < 6; i++)
		if (cnt[i] >= 2)
			throw	NotValidConfigFileException();
	if (CloseBraceCnt != 1)
	{
		std::cout << "dasfcv" << std::endl;
		throw	NotValidConfigFileException();
	}
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
}

int Http::ft_stoi(const std::string &str)
{
    int ret = 0;

    for (int i = 0; str[i]; i++)
    {
        if (!('0' <= str[i] && str[i] <= '9') || ret < 0)
            throw NotValidConfigFileException();
		ret *= 10;
		ret += str[i] - '0';
    }
	return (ret);
}

void    Http::SettingHttp(void)
{

}

void    Http::ExitHttpError(const std::string &msg) const
{
    std::cerr << msg << std::endl;
    exit(1);
}

void    Http::printServerInfo(const unsigned short &port)
{
	try
	{
		ServerBlock server = getServer(port);
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
			std::cout << "    }" << std::endl;
		}
        std::cout << "}" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

ServerBlock Http::getServer(const int &port)
{
	std::vector<std::pair<unsigned short, ServerBlock> >::iterator it;
	for (it = this->server_block.begin(); it != this->server_block.end() && it->first != port; it++);
	if (it == this->server_block.end())
		throw NoSuchServerPort();
	return (it->second);
}

LocationBlock Http::getLocation(const std::string &default_root, ServerBlock &server)
{
	std::vector<std::pair<std::string, LocationBlock> >::iterator it;
	for (it = server.location_block.begin(); it != server.location_block.end() && it->first.compare(default_root); it++);
	if (it == server.location_block.end())
		throw NoSuchLocationBlock();
	return (it->second);
}

const char *Http::NotValidConfigFileException::what() const throw()
{
    return ("Error : Not Valid Configuration File");
}
const char *Http::SettingHttpExcecption::what() const throw()
{
    return ("Error : Http Setting");
}
const char *Http::NoSuchFileException::what() const throw()
{
	return ("Error : No Such File");
}

const char *Http::NoSuchServerPort::what() const throw()
{
	return ("Error : No Such Server Port");
}

const char *Http::NoSuchLocationBlock::what() const throw()
{
	return ("Error : No Such Location Block");
}