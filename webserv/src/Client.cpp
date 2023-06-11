#include "../header/Http.hpp"

void    Http::clientInit(struct sockaddr_in clnt_adr, int clnt_sock) {
	clients[clnt_sock].host = 80;
    clients[clnt_sock].str_len = 0;
	clients[clnt_sock].root = "";
	clients[clnt_sock].method = "";
    clients[clnt_sock].request = "";
	clients[clnt_sock].http_ver = "";
    clients[clnt_sock].clnt_adr = clnt_adr;
    clients[clnt_sock].last_active_times = std::time(NULL);
	std::cout << "client connected : " << clnt_sock << " [" << ft_ntohs(clients[clnt_sock].clnt_adr.sin_port) << "]" << std::endl;
}

void	Http::eventErrHandler(int serv_sock, int clnt_sock) {
	if (clnt_sock == serv_sock) {
		occurException(12, "socket", HTTP, CLIENT,
        "socket error");
	} else {
		std::cerr << "client socket error" << std::endl;
		disconnectClient(clnt_sock);
	}
}

void	Http::eventReadHandler(int serv_sock, int clnt_sock, ServerBlock &server) {
	if (clnt_sock == serv_sock)
		clientAccept(serv_sock, clnt_sock, server);
	else if (clients.find(clnt_sock) != clients.end())
		readRequestMsg(clnt_sock);
}

void	Http::disconnectClient(int clnt_sock) {
	std::cout << "client disconnected : " << clnt_sock << " [" << ft_ntohs(clients[clnt_sock].clnt_adr.sin_port) << "]"<< std::endl;
	close(clnt_sock);
	clients.erase(clnt_sock);
}

void	Http::clientAccept(int serv_sock, int clnt_sock, ServerBlock &server) {
	int					clnt_sock;
	struct sockaddr_in	clnt_adr;
	socklen_t			clnt_adr_size;

	clnt_adr_size = sizeof(clnt_adr);
	if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1) {
        occurException(44, "accept()", HTTP, CLIENT,
        "accept function error");
    }
    clientInit(clnt_adr, clnt_sock);
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
	EV_SET(&server.chagelist, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	EV_SET(&server.chagelist, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Http::readRequestMsg(int clnt_sock) {
	char	buf[BUF_SIZE];
	ssize_t	n;

	n = read(clnt_sock, buf, BUF_SIZE);
	if (n == -1) {
		disconnectClient(clnt_sock);
		return ;
	} else if (n) {
		buf[n] = '\0';
		clients[clnt_sock].str_len += n;
		clients[clnt_sock].last_active_times = std::time(NULL);
		clients[clnt_sock].request += static_cast<std::string>(buf);
	}
}

void	Http::writeResponse(int clnt_sock) {
	std::pair<std::string, std::string>	response;

	err = 0;
	response.first = getMsg(clnt_sock);
	response.second = getContent(clnt_sock);
	if (err)
		response.first = buildErrorMsg(clnt_sock);
	if ((write(clnt_sock, response.first.c_str(), response.first.length())) == -1)
		std::cerr << "Error : write error (response msg)" << std::endl;
	if ((write(clnt_sock, response.second.c_str(), response.second.length())) == -1)
		std::cerr << "Error : write error (response content)" << std::endl;
	close(clnt_sock);
}

void	Http::clientHandler() {
	ServerBlock			server;

	for (int i = 0; i < nevents; i++) {
		curr_event = &evlist[i];
		for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
			server = *it;
			if (curr_event->flags & EV_ERROR)
				eventErrHandler(server.serv_sock, curr_event->ident);
			else if (curr_event->filter == EVFILT_READ)
				eventReadHandler(server.serv_sock, curr_event->ident, server);
			else if (curr_event->filter == EVFILT_WRITE)
				writeResponse(curr_event->ident);
		}
	}
}