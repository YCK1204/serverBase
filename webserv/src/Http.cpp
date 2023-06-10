#include "../header/Http.hpp"

volatile sig_atomic_t flag = 0;
void	handle_signal(int signum) { signum = 1;}
Http::Http() {}
Http::Http(const std::string &path)
{
	ParsingConfig(path);
	printConfigInfo();
	checkValidConfig();
	SettingHttp();
	//runServer();
}
Http::~Http() {}

void    Http::SettingHttp()
{
	int	reuse = 1;
	if ((this->kq = kqueue()) == -1)
		serverFunctionExecuteFailed(19, "kqueue()");
	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		ServerBlock &server = *it;

		if ((server.serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			serverFunctionExecuteFailed(24, "kqueue()");
		std::memset(&server.serv_adr, 0, sizeof(server.serv_adr));
		server.serv_adr.sin_family = AF_INET;
		server.serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
		server.serv_adr.sin_port = htons(server.port);
		if (setsockopt(server.serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
			serverFunctionExecuteFailed(30, "setsocketopt()");
		if (bind(server.serv_sock, (struct sockaddr *)&server.serv_adr, sizeof(server.serv_adr)) == -1)
			serverFunctionExecuteFailed(32, "bind()");
		if (listen(server.serv_sock, LISTEN_SIZE))
			serverFunctionExecuteFailed(34, "listen()");
		EV_SET(&server.chagelist, server.serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
			serverFunctionExecuteFailed(37, "kevent()");
	}
}
/*
void    Http::runServer()
{
	int	clnt_sock, nevents, sockfd;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_size;
	struct kevent evlist[MAX_EVENTS];
	std::string tmp;
	bool		acpt;
	ServerBlock	server;
	ssize_t	str_len;

	while (!flag) {
		if ((nevents = kevent(this->kq, NULL, 0, evlist, MAX_EVENTS, NULL)) == -1)
			occurException("kevent()", SERVER);
		for (int i = 0; i < nevents; i++) {
			sockfd = evlist[i].ident;
			acpt = false;
			for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++) {
				server = it->second;
				if (sockfd == server.serv_sock) {
					clnt_adr_size = sizeof(clnt_adr);
					if ((clnt_sock = accept(sockfd, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1)
						occurException("accept()", SERVER);
					EV_SET(&server.chagelist, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					EV_SET(&server.chagelist, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
						occurException("kevent()", SERVER);
					acpt = true;
				}
			} if (!acpt) {
				char req_line[BUF_SIZE];
				str_len = read(sockfd, req_line, BUF_SIZE);
				if (str_len == 0) {
					close(sockfd);
					continue ;
				}
				std::stringstream ss(req_line);
				std::getline(ss, tmp);
				std::getline(ss, tmp);
				std::cout << req_line << std::endl;
				send_data(sockfd, req_line, tmp, str_len);
				close(sockfd);
			}
		}
	}
	for (std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = this->server_block.begin(); it != this->server_block.end(); it++)
		close(it->second.serv_sock);
}

void	Http::send_data(int clnt_sock, char *msg, std::string &host, ssize_t &str_len) {
	const char	*ResponseMsg;
	const char	*file;
	std::string tmp;
	std::pair<std::string, std::string>	data;
	std::vector<std::pair<unsigned short, ServerBlock> >::iterator it = getServer(static_cast<unsigned short>(std::atoi(host.substr(host.rfind(":") + 1).c_str())));

	if (it != this->server_block.end() && str_len > it->second.client_body_size) {
		std::vector<std::pair<std::string, LocationBlock> >::iterator temp = it->second.location_block.begin();
		data.first = setResponseLine(temp, it,
			413, "Payload Too Large"
		);
		data.second = makeHtml("<h1> 413에러났죠? ㅋㅋ <br>그렇게 하는거 아닌데 ㅋ</h1>\n");
	} else {
		data = makeResponse(it, msg);
	}
	file = data.second.c_str();
	data.first += "Content-length:" + ft_to_string(std::strlen(file)) + "\r\n\r\n";
	ResponseMsg = data.first.c_str();
	write(clnt_sock, ResponseMsg, std::strlen(ResponseMsg));
	write(clnt_sock, file, std::strlen(file));
}*/

void	Http::serverFunctionExecuteFailed(const int line, std::string msg) {
	occurException(line, msg, HTTP, F_HTTP,
	"[" + static_cast<const std::string>(strerror(errno)) + "]");
}