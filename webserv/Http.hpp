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
#include <algorithm>

enum s_block_type
{
    LISTEN,
    ERROR_PAGE,
    HOST,
    BODY_SIZE,
    S_INDEX,
    S_ROOT,
    SERVER_NAME
};

enum methods
{
    GET,
    POST,
    DELETE
};

enum l_block_type
{
    METHOD,
    AUTOINDEX,
    L_ROOT,
    L_INDEX,
    RETURN,
    CGI
};

typedef struct
{
	bool                        methods[3];
	bool						autoindex;
	bool						ret;
	std::string					root;
	std::string					index;
	std::string					cgi_bin;
	std::string					redirect;
	std::string					default_root;
    std::string                 index_root;
}	LocationBlock;

typedef struct
{
    std::vector<std::pair<std::string, LocationBlock> > location_block;
    std::string                 error_page;
	std::string					root;
	std::string					server_name;
    std::string					index;
    std::string                 host;
	int   		            	client_body_size;
    unsigned short              port;
    std::string                 index_root;
}	ServerBlock;

class Http
{
private:
    int serv_sock, clnt_sock, fd_max, read_len, fd_num, i;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timespec timeout;
    socklen_t adr_sz;
    std::vector<std::pair<unsigned short, ServerBlock> >	server_block;

    Http();
    Http &operator = (const Http &s);

    int     ft_stoi(const std::string &str);

    void    checkExistFile();
    void    ParsingConfig(const std::string &path);
    void    checkValidConfig();
    void    checkOverllapServerPort(const unsigned short &port);
    void    checkOverllapLocationRoot(const std::string &root, ServerBlock &server);
    void	checkValidAddr(const std::string &host);

    void	                                server_block_argu_split(std::stringstream &ss, s_block_type t, ServerBlock &ret);
    std::pair<unsigned short, ServerBlock>	Server_split(std::ifstream &config);
    void	                                location_block_argu_split(std::stringstream &ss, l_block_type t, LocationBlock &ret);
    std::pair<std::string, LocationBlock>	location_block_split(std::ifstream &config, std::string &default_root);
    ServerBlock                             getServer(const int &port);
    LocationBlock                           getLocation(const std::string &default_root, ServerBlock &server);

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
    class NoSuchServerPort :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class NoSuchLocationBlock :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class EmptyFileException :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class ServerPortOverllapException :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class LocationRootOverllapException :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class noSuchFileException :  public std::exception
    {
        public:
            const char *what() const throw();
    };
    class notValidAddrException :  public std::exception
    {
        public:
            const char *what() const throw();
    };
public:
    Http(const std::string &path);
    ~Http();

    void        SettingHttp(void);
    void        ExitHttpError(const std::string &msg) const;
    void        printConfigInfo();
};

#endif