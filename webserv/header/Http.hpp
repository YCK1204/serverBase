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
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#define LISTEN_SIZE 20
#define BUF_SIZE 2048
#define SMALL_BUF 2048
#define MAX_EVENTS 100

enum exception
{
    CONFIG,
    FILEROOT,
    EMPTY,
    PORT,
    ROOT,
    ADDR,
    HTTP,
    SERVER
};

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
    int                         serv_sock;
    struct sockaddr_in          serv_adr;
    struct kevent               chagelist;
}	ServerBlock;

class Http
{
private:
    int kq;
    std::vector<std::pair<unsigned short, ServerBlock> >	server_block;

    Http();
    Http &operator = (const Http &s);

    int         ft_stoi(const std::string &str);
    uint16_t    ft_ntohs(uint16_t port);
    std::string ft_inet_ntoa(uint32_t ipaddr);

    void        checkExistFile();
    void        ParsingConfig(const std::string &path);
    void        checkValidConfig();
    void        checkOverlapServerPort(const unsigned short &port);
    void        checkOverlapLocationRoot(const std::string &root, ServerBlock &server);
    void	    checkValidAddr(const std::string &host);
    void        occurException(const std::string &msg, exception type);
    
    std::string	findRoot(ServerBlock &server, std::string file_name);
    void        runServer();
    void        send_data(int clnt_sock, std::string file_name);
    void        SettingHttp();
    void        send_error(int clnt_sock);

    std::vector<std::pair<std::string, std::string> >   makeResponse(ServerBlock &server, std::stringstream &ss);
    std::string                                         checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, ServerBlock &server, std::vector<std::pair<std::string, LocationBlock> >::iterator it);
    std::string                                         setResponseLine(LocationBlock &location, size_t const &ResponseCode);

    void	                                server_block_argu_split(std::stringstream &ss, s_block_type t, ServerBlock &ret);
    void	                                location_block_argu_split(std::stringstream &ss, l_block_type t, LocationBlock &ret);
    std::pair<unsigned short, ServerBlock>	Server_split(std::ifstream &config);
    std::pair<std::string, LocationBlock>	location_block_split(std::ifstream &config, std::string &default_root);

    class   NotValidConfigFileException : public std::exception{
        public:
            const char *what() const throw();
    };
    class NoSuchFileException : public std::exception{
        public:
            const char *what() const throw();
    };
    class EmptyFileException :  public std::exception{
        public:
            const char *what() const throw();
    };
    class ServerPortOverlapException :  public std::exception{
        public:
            const char *what() const throw();
    };
    class LocationRootOverlapException :  public std::exception{
        public:
            const char *what() const throw();
    };
    class notValidAddrException :  public std::exception{
        public:
            const char *what() const throw();
    };
    class SettingHttpException :  public std::exception{
        public:
            const char *what() const throw();
    };
    class RunServerException :  public std::exception{
        public:
            const char *what() const throw();
    };
public:
    Http(const std::string &path);
    ~Http();

    void        printConfigInfo();
};

#endif