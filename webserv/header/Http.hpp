#ifndef HTTP_HPP
# define HTTP_HPP

#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include <ctime>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../header/Mime.hpp"

#define LISTEN_SIZE 20
#define BUF_SIZE 2048
#define MAX_EVENTS 100

enum files {
    F_HTTP,
    PARSING,
    S_PARSING,
    L_PARSING,
    UTIL,
    CLIENT
};

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
	bool						                ret;
	bool						                autoindex;
	bool                                        methods[3];

	std::string					                cgi;
	std::string					                root;
	std::string					                index;
	std::string					                redirect;
    std::string                                 index_root;
	std::string					                default_root;
}	LocationBlock;

typedef struct {
    struct kevent                               chagelist;
    struct sockaddr_in                          serv_adr;
    std::vector<LocationBlock>                  location;
	std::string					                root;
    std::string                                 host;
    std::string					                index;
    std::string                                 error_page;
	std::string					                server_name;
    std::string                                 index_root;
    int                                         port;
    int                                         serv_sock;
	int   		            	                client_body_size;
}	ServerBlock;

typedef struct {
    int                                         host;
    std::string                                 root;
    std::string                                 method;
    std::string                                 request;
    std::string                                 http_ver;
    struct sockaddr_in                          clnt_adr;
    ssize_t                                     str_len;
    std::time_t                                 last_active_times;
}   ClientData;

class Http {
private:
    std::vector<ServerBlock>                    server;
    std::map<int, ClientData>                   clients;
    int                                         kq, clnt_sock, nevents, err;
	struct kevent                               evlist[MAX_EVENTS];
	struct kevent                               *curr_event;
    Mime                                        mime;

    Http();
    Http &operator = (const Http &s);

    /* exception_functions*/
    void                                        exception_util(const std::string &msg, exception type);
    void	                                    serverFunctionExecuteFailed(const int line, std::string msg);
    void	                                    serverFunctionExecuteFailed(const int line, std::string msg, std::string detail);
    void	                                    occurException(const int &line, const std::string &msg, exception type, files file, const std::string &reason);
    /* exception_functions*/

    /* parsing_functions*/
    std::string                                 buildLocationOption(std::stringstream &ss);
    std::string                                 buildServerOption(std::ifstream &file, std::stringstream &ss, s_block_type type);

    ServerBlock                                 makeServer(std::ifstream &file);
    LocationBlock                               makeLocation(std::ifstream &file, std::stringstream &ss);

    void                                        checkValidConfig();
    void                                        checkValidServerOption(int types[7]);
    void                                        ParsingConfig(const std::string &path);
    void	                                    checkEmptyFile(const std::string &path);
    void	                                    checkValidAddr(const std::string &host);
    void                                        buildLocationOption(std::stringstream &ss, bool types[3]);
    void	                                    checkValidateAddress(int addr[4], const std::string &host);
    void                                        setServerOption(std::ifstream &file, std::stringstream &ss, std::string cmd, ServerBlock &ret, int types[7]);

    int                                         distinctionMethods(std::string &method);
    bool                                        buildLocationOption(std::stringstream &ss, std::string tt);
    /* parsing_functions*/

    /* util_functions */
    std::string                                 ft_to_string(int n);
    std::string                                 ft_inet_ntoa(uint32_t ipaddr);

    void                                        printConfigInfo();
    void                                        recursive_to_string(int n, std::string &ret);
    void	                                    exception_util(const std::string &str, s_block_type type);

    uint16_t                                    ft_ntohs(uint16_t port);
    bool	                                    ExistFile(std::string &root);
    bool	                                    ExistDirectory(std::string &root);
    int                                         ft_stoi(const std::string &str, s_block_type type);
    /* util_functions */

    /* server_functions */
    void                                        runServer();
    void                                        SettingHttp();
    /* server_functions */

    /* client_functions*/
    void	                                    clientHandler();
    void	                                    writeResponse(int clnt_sock);
    void	                                    readRequestMsg(int clnt_sock);
    void	                                    disconnectClient(int clnt_sock);
    void	                                    eventErrHandler(int serv_sock, int clnt_sock);
    void                                        clientInit(struct sockaddr_in clnt_adr, int clnt_sock);
    void	                                    clientAccept(int serv_sock, int clnt_sock, ServerBlock &server);
    void	                                    eventReadHandler(int serv_sock, int clnt_sock, ServerBlock &server);
    /* client_functions*/

    /* html_functions*/
    std::string                                 buildHtml(const std::string msg);
    std::string                                 buildErrorHtml(const int status);
    /* html_functions*/

    /* response_functions */
    std::string                                 &getMsg(int clnt_sock);
    std::string                                 &getContent(int clnt_sock);
    std::string                                 &buildErrorMsg(int clnt_sock);


    std::string                                 makeAutoindex(std::string root);
    std::pair<std::string, std::string>         makeResponse(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, char *msg);
    std::string                                 readFile(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt, std::string &msg);
    std::string                                 setResponseLine(std::vector<std::pair<std::string, LocationBlock> >::iterator &location, std::vector<std::pair<unsigned short, ServerBlock> >::iterator server, size_t const &ResponseCode, std::string msg);
    std::string                                 checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt);
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

};

#endif