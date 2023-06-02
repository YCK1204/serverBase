#include "Http.hpp"

Http::Http() {}
Http::~Http() {}


void	Http::server_block_argu_split(std::stringstream &ss, type t, ServerBlock &ret)
{
	std::string tmp, temp;
	int			val;

	ss >> tmp >> temp;

	if (temp.length() || ((t == LISTEN || t == BODY_SIZE) && (val = ft_stoi(tmp)) > 65535))
		throw	NotValidConfigFileException();
	if (t == LISTEN)
		ret.port = static_cast<unsigned short>(val);
	else if (t == ROOT)
		ret.root = tmp;
	else if (t == SERVER_NAME)
		ret.server_name = tmp;
	else if (t == ERROR_PAGE)
		ret.error_page = tmp;
	else if (t == INDEX)
		ret.index = tmp;
	else if (t == BODY_SIZE)
		ret.client_body_size = val;
}

std::pair<std::string, LocationBlock>	Http::location_block_split(std::ifstream &config)
{
	LocationBlock	ret;
}

std::pair<unsigned short, ServerBlock>	Http::Server_split(std::ifstream &config)
{
	ServerBlock	ret;
	std::string	line, cmd, temp, tmp;
	int			OpenBraceCnt = 1, CloseBraceCnt = 0;
	int			cnt[6] = {};

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
		else if (!cmd.compare("listen") && ++cnt[LISTEN])
			server_block_argu_split(ss, LISTEN, ret);
		else if (!cmd.compare("root") && ++cnt[ROOT])
			server_block_argu_split(ss, ROOT, ret);
		else if (!cmd.compare("server_name") && ++cnt[SERVER_NAME])
			server_block_argu_split(ss, SERVER_NAME, ret);
		else if (!cmd.compare("error_page") && ++cnt[ERROR_PAGE])
			server_block_argu_split(ss, ERROR_PAGE, ret);
		else if (!cmd.compare("index") && ++cnt[INDEX])
			server_block_argu_split(ss, INDEX, ret);
		else if (!cmd.compare("client_body_size") && ++cnt[BODY_SIZE])
			server_block_argu_split(ss, BODY_SIZE, ret);
		else if (!cmd.compare("location") && ++OpenBraceCnt)
		{
			ss >> temp >> tmp;
			if (temp.compare("{") || tmp.length())
				throw	NotValidConfigFileException();
			ret.location.push_back(location_block_split(config));
		}
	}
	for (int i = 0; i < 6; i++)
		if (cnt[i] >= 2)
			throw	NotValidConfigFileException();
	if (OpenBraceCnt != CloseBraceCnt)
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

const char *Http::NotValidConfigFileException::what() const throw()
{
    return ("Error : Config file");
}
const char *Http::SettingHttpExcecption::what() const throw()
{
    return ("Error : Http Setting");
}
const char *Http::NoSuchFileException::what() const throw()
{
	return ("Error : No Such File");
}
