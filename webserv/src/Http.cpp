#include "../header/Http.hpp"

volatile sig_atomic_t flag = 0;
void	handle_signal(int signum) { signum = 1;}
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

void    Http::SettingHttp()
{
	int	reuse = 1;
	if ((this->kq = kqueue()) == -1)
		occurException("kqueue()", HTTP);
	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++) {
		ServerBlock &server = it->second;

		if ((server.serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			occurException("socket()", HTTP);
		std::memset(&server.serv_adr, 0, sizeof(server.serv_adr));
		server.serv_adr.sin_family = AF_INET;
		server.serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
		server.serv_adr.sin_port = htons(server.port);
		setsockopt(server.serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
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

	while (!flag) {
		if ((nevents = kevent(this->kq, NULL, 0, evlist, MAX_EVENTS, NULL)) == -1)
			occurException("kevent()", SERVER);
		for (int i = 0; i < nevents; i++) {
			sockfd = evlist[i].ident;
			for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++) {
				ServerBlock &server = it->second;
				if (sockfd == server.serv_sock) {
					clnt_adr_size = sizeof(clnt_adr);
					if ((clnt_sock = accept(sockfd, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1)
						occurException("accept()", SERVER);
					std::cout << "Connection Request : " << ft_inet_ntoa(clnt_adr.sin_addr.s_addr) << " : " << ft_ntohs(clnt_adr.sin_port) << std::endl;
					EV_SET(&server.chagelist, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					EV_SET(&server.chagelist, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
						occurException("kevent()", SERVER);
				} else {
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
						std::cout << "line : " << line << std::endl;
					req_msg >> method >> file_name;
					std::cout << "req line" << req_line << std::endl;

					std::cout << "method" << method << std::endl;
					std::cout << "file_name" << file_name << std::endl;
					req_msg >> method;
					std::cout << "HTTP" << method << std::endl;
					req_msg >> method;
					std::cout << "ver" << method << std::endl;
					req_msg >> method;
					std::cout << "HTTP" << method << std::endl;
					if (method.compare("GET")) {
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

void Http::send_error(int clnt_sock) { 
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

std::string	Http::findRoot(ServerBlock &server, std::string file_name) {
	for (std::vector<std::pair<std::string, LocationBlock> >::iterator it = server.location_block.begin(); it != server.location_block.end(); it++) {
		std::cout << "location root : " << it->second.default_root << ", file_name : " << file_name << std::endl;
		if (!it->second.default_root.compare(file_name))
			return (it->second.index_root);
	}
	return ("");
}

void    Http::send_data(int clnt_sock, std::string file_name) {
	char protocol[]="HTTP/1.1 200 OK\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]="Content-length:2048\r\n";
	const char *type;
	std::string ct = "text/html";
	
	std::string tmp;
	std::string cnt_type = "Content-type:" + ct + "\r\n\r\n";
	std::ifstream file;

	std::cout << "파일 네임 : " + file_name << std::endl;
	file.open(file_name.c_str());
    if(!file.is_open()) {
		std::cout << "에러 발생 !!!!!!!!!!!!!! " << std::endl;
        send_error(clnt_sock);
        return;
    }
	type = cnt_type.c_str();

    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, type, std::strlen(type));
	std::cout << "this" << std::endl;
	while (std::getline(file, tmp)) {
		tmp += '\n';
		const char *msg = tmp.c_str();
		std::cout << tmp << std::endl;
		write(clnt_sock, msg, std::strlen(msg));
	}
	std::cout << "this" << std::endl;
	file.close();
}