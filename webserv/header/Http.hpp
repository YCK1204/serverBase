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
#define TIMEOUT 2
#define MAX_ROOT_LEN 30

#define RED "\033[31m"
#define ORANGE "\033[33m"
#define YELLOW "\033[93m"
#define LIME "\033[92m"
#define SKY_BLUE "\033[96m"

#define STDOUT 1
#define STDERR 2

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
	std::string					                default_root;
}	LocationBlock;

typedef struct {
    struct sockaddr_in                          serv_adr;

    std::vector<LocationBlock>                  location;
    
	std::string					                root;
    std::string                                 host;
    std::string					                index;
    std::string                                 error_page;
    std::string                                 index_root;
	std::string					                server_name;

    int                                         port;
    int                                         serv_sock;
	int   		            	                client_body_size;
}	ServerBlock;

typedef struct {
    int                                         port;
    int                                         addr[4];

    std::string                                 root;
    std::string                                 http;
    std::string                                 method;
    std::string                                 request;
    std::string                                 http_ver;
    std::string                                 connection;
    std::string                                 file_extension;

    ssize_t                                     str_len;
    std::time_t                                 last_active_times;
}   ClientData;

struct FileInfo {
    std::string                                 name;
    off_t                                       size;
    time_t                                      lastModified;
    bool                                        is_dir;
};

class Http {
private:
    fd_set                                      events, read_event, write_event, err_event;
    std::vector<ServerBlock>                    server;
    std::map<int, ClientData>                   clients;
    int                                         err, fd_max, status;
    Mime                                        mime;
    std::vector<struct kevent>                  changeList;


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
    void                                        setServerOption(std::ifstream &file, std::stringstream &ss, std::string cmd, ServerBlock &ret, int types[7]);

    int                                         distinctionMethods(std::string &method);
    bool                                        buildLocationOption(std::stringstream &ss, std::string tt);
    /* parsing_functions*/

    /* util_functions */
    ServerBlock                                 getServer(std::string host, std::string root);
    LocationBlock                               getLocation(std::string root, ServerBlock server);

    std::string                                 getDate();
    std::string                                 ft_to_string(int n);
    std::string                                 formatSize(double size);
    std::string                                 spaceTrim(std::string str);
    std::string                                 ft_inet_ntoa(uint32_t ipaddr);
    std::string                                 formatTime(const time_t& time);
    std::string                                 buildAutoindex(std::string server_root, std::string location_root);

    void                                        printConfigInfo();
    void	                                    addFdSet(int clnt_sock, fd_set &event);
    void	                                    clearFdSet(int clnt_sock, fd_set &event);
    void                                        recursive_to_string(int n, std::string &ret);
    void                                        printLog(std::string color, std::string msg, int fd);
    void	                                    exception_util(const std::string &str, s_block_type type);

    uint16_t                                    ft_ntohs(uint16_t port);

    bool                                        isDir(std::string dir_root);
    bool	                                    ExistFile(std::string &root);
    bool                                        isFile(std::string file_root);
    bool	                                    checkValidateAddress(int addr[4]);
    bool	                                    ExistDirectory(std::string &root);

    int                                         ft_stoi(const std::string &str);
    int                                         ft_stoi(const std::string &str, s_block_type type);
    /* util_functions */

    /* server_functions */
    void                                        runServer();
    void                                        SettingHttp();
    void	                                    initializeServer();
    /* server_functions */

    /* client_functions*/
    void	                                    clientHandler();
    void	                                    writeResponse(int clnt_sock);
    void	                                    readRequestMsg(int clnt_sock);
    void	                                    eventErrHandler(int clnt_sock);
    void	                                    eventReadHandler(int clnt_sock);
    void	                                    disconnectClient(int clnt_sock);
    void                                        clientInit(int clnt_sock);
    void	                                    clientAccept(int serv_sock, int clnt_sock, ServerBlock &server);
    /* client_functions*/

    /* html_functions*/
    std::string                                 buildErrorMsg(int clnt_sock);
    std::string                                 buildHtml(const std::string msg);
    std::string                                 buildErrorHtml(const int status);
    /* html_functions*/

    /* response_functions */
    std::pair<std::string, std::string>         getResponse(int clnt_sock);

    std::string                                 getMsg(int clnt_sock, int length);
    std::string                                 getContent(int clnt_sock);
    std::string                                 getRoot(std::string req_msg);
    std::string                                 getHTTP(std::string req_msg);
    std::string                                 getPort(std::string req_msg);
    std::string                                 getMethod(std::string req_msg);
    std::string                                 getAddress(std::string req_msg);
    std::string                                 getHttpVer(std::string req_msg);
    std::string                                 getConnection(std::string req_msg);
    std::string                                 getIndexRoot(ServerBlock server, LocationBlock location, int clnt_sock);

    void                                        getData(int clnt_sock);
    void                                        checkRequestMsg(int clnt_sock);

    bool                                        isAutoindex(int clnt_sock);
    bool                                        isValidAddress(ServerBlock server, int addr[4]);
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