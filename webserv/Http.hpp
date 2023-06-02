#ifndef HTTP_HPP
# define HTTP_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sys/types.h>
#include <vector>
#include <iterator>
#include <fstream>
#include <sstream>

enum type
{
    LISTEN,
    ROOT,
    SERVER_NAME,
    ERROR_PAGE,
    INDEX,
    BODY_SIZE
};

typedef struct
{
	std::vector<std::string>	allow_methods;
	bool						autoindex;
	bool						ret;
	std::string					root;
	std::string					index;
	std::string					cgi_bin;
	std::string					redirect;
	std::string					default_root;
}	LocationBlock;

typedef struct
{
	std::vector<std::pair<std::string, LocationBlock> > location;
    std::string                 error_page;
	std::string					root;
	std::string					server_name;
    std::string					index;
	int         				client_body_size;
    unsigned short              port;
}	ServerBlock;

class Http
{
private:
    int serv_sock, clnt_sock, fd_max, read_len, fd_num, i;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timespec timeout;
    socklen_t adr_sz;
    std::vector<std::pair<int, ServerBlock> >	server_block;

    Http(const Http &s);
    Http &operator = (const Http &s);

    int     ft_stoi(const std::string &str);
    void	server_block_argu_split(std::stringstream &ss, type t, ServerBlock &ret);
    std::pair<std::string, LocationBlock>	location_block_split(std::ifstream &config);
    std::pair<unsigned short, ServerBlock>	Server_split(std::ifstream &config);
    class   NotValidConfigFileException : public std::exception
    {
        public:
            const char *what() const throw();
    };
    class NoSuchFileException : public std::exception
    {
        public:
            const char *what() const throw();
    };
    class SettingHttpExcecption : public std::exception
    {
        public:
            const char *what() const throw();
    };
public:
    Http();
    ~Http();
    void    ParsingConfig(const std::string &path);
    void    SettingHttp(void);
    void    ExitHttpError(const std::string &msg) const;
};

#endif
