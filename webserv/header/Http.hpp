#ifndef HTTP_HPP
# define HTTP_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#define LISTEN_SIZE 20
#define BUF_SIZE 2048
#define SMALL_BUF 2048
#define MAX_EVENTS 100

enum exception {
    CONFIG,
    FILEROOT,
    EMPTY,
    PORT,
    ROOT,
    ADDR,
    HTTP,
    SERVER
};

enum s_block_type {
    LISTEN,
    ERROR_PAGE,
    HOST,
    BODY_SIZE,
    S_INDEX,
    S_ROOT,
    SERVER_NAME
};

enum methods {
    GET,
    POST,
    DELETE
};

enum l_block_type {
    METHOD,
    AUTOINDEX,
    L_ROOT,
    L_INDEX,
    RETURN,
    CGI
};

typedef struct {
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

typedef struct {
    std::vector<std::pair<std::string, LocationBlock> > location_block;
    std::string                 error_page;
	std::string					root;
	std::string					server_name;
    std::string					index;
    std::string                 host;
    std::string                 index_root;
    int                         serv_sock;
	int   		            	client_body_size;
    unsigned short              port;
    struct sockaddr_in          serv_adr;
    struct kevent               chagelist;
}	ServerBlock;

class Http {
private:
    int kq;
    std::vector<std::pair<unsigned short, ServerBlock> >	server_block;

    Http();
    Http &operator = (const Http &s);

    /* util_functions */
    uint16_t                                                        ft_ntohs(uint16_t port);
    std::string                                                     ft_inet_ntoa(uint32_t ipaddr);
    int                                                             ft_stoi(const std::string &str);
    bool	                                                        ExistFile(std::string &root);
    std::string                                                     ft_to_string(int n);
    void                                                            recursive_to_string(int n, std::string &ret);
    std::vector<std::pair<unsigned short, ServerBlock> >::iterator  getServer(const unsigned short &port);
    /* util_functions */

    /* parsing_functions */
    void                                    checkExistFile();
    void                                    checkValidConfig();
    void                                    checkOverlapLocationRoot(const std::string &root, ServerBlock &server);
    void	                                checkValidAddr(const std::string &host);
    void                                    checkOverlapServerPort(const unsigned short &port);
    void                                    occurException(const std::string &msg, exception type);
    void                                    ParsingConfig(const std::string &path);
    std::pair<std::string, LocationBlock>	location_block_split(std::ifstream &config, std::string &default_root);
    void	                                location_block_argu_split(std::stringstream &ss, l_block_type t, LocationBlock &ret);
    void	                                server_block_argu_split(std::stringstream &ss, s_block_type t, ServerBlock &ret);
    std::pair<unsigned short, ServerBlock>	Server_split(std::ifstream &config);
    /* parsing_functions */
    
    /* server_functions */
    void                                    runServer();
    void	                                send_data(int clnt_sock, char *msg, std::string &host, ssize_t &str_len);
    void                                    SettingHttp();
    /* server_functions */

    /* response_functions */
    std::string                             makeHtml(const std::string msg);
    std::string                             makeAutoindex(std::string root);
    std::pair<std::string, std::string>     makeResponse(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, char *msg);
    std::string                             readFile(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt, std::string &msg);
    std::string                             setResponseLine(std::vector<std::pair<std::string, LocationBlock> >::iterator &location, std::vector<std::pair<unsigned short, ServerBlock> >::iterator server, size_t const &ResponseCode, std::string msg);
    std::string                             checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt);
    /* response_functions */
    

    class   NotValidConfigFileException : public std::exception {
        public:
            const char *what() const throw();
    };
    class NoSuchFileException : public std::exception {
        public:
            const char *what() const throw();
    };
    class EmptyFileException : public std::exception {
        public:
            const char *what() const throw();
    };
    class ServerPortOverlapException : public std::exception {
        public:
            const char *what() const throw();
    };
    class LocationRootOverlapException : public std::exception {
        public:
            const char *what() const throw();
    };
    class notValidAddrException : public std::exception {
        public:
            const char *what() const throw();
    };
    class SettingHttpException : public std::exception {
        public:
            const char *what() const throw();
    };
    class RunServerException : public std::exception {
        public:
            const char *what() const throw();
    };
public:
    Http(const std::string &path);
    ~Http();

    void        printConfigInfo();
};

#endif