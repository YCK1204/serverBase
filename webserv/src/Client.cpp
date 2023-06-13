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

void	Http::eventErrHandler(int clnt_sock) {
	ServerBlock			server;

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		server = *it;
		if (clnt_sock == server.serv_sock) {
			occurException(12, "socket", HTTP, CLIENT,
			"socket error");
		}
	}
	std::cerr << "client socket error" << std::endl;
	disconnectClient(clnt_sock);
}

void	Http::eventReadHandler(int clnt_sock) {
	bool	t = false;
	ServerBlock			server;

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		server = *it;
		if (clnt_sock == server.serv_sock) {
			clientAccept(server.serv_sock, clnt_sock, server);
			t = true;
			break ;
		}
	}
	if (!t) {
		if (clients.find(clnt_sock) != clients.end()) {
			readRequestMsg(clnt_sock);
		}
	}

}

void	Http::disconnectClient(int clnt_sock) {
	std::cout << "client disconnected : " << clnt_sock << " [" << ft_ntohs(clients[clnt_sock].clnt_adr.sin_port) << "]"<< std::endl;
	close(clnt_sock);
	clients.erase(clnt_sock);
	FD_CLR(clnt_sock, &events);
}

void	Http::clientAccept(int serv_sock, int clnt_sock, ServerBlock &server) {
	int					clnt_sk;
	struct sockaddr_in	clnt_adr;
	socklen_t			clnt_adr_size;
	struct kevent		temp;

	clnt_adr_size = sizeof(clnt_adr);
	if ((clnt_sk = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1) {
        occurException(44, "accept()", HTTP, CLIENT,
        "accept function error");
    }
    clientInit(clnt_adr, clnt_sk);
	fcntl(clnt_sk, F_SETFL, O_NONBLOCK);
	FD_SET(clnt_sk, &events);
	if (fd_max < clnt_sk)
		fd_max = clnt_sk;

	// change_events(changeList, clnt_sk, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void Http::readRequestMsg(int clnt_sock) {
    ssize_t n;
    int sock;

    sock = clnt_sock;
    char* buf = new char[BUF_SIZE];
    n = read(clnt_sock, buf, BUF_SIZE);
    if (n == -1) {
        disconnectClient(clnt_sock);
        delete[] buf;
        return;
    } else if (n) {
        buf[n] = '\0';
        clients[clnt_sock].str_len += n;
        clients[clnt_sock].last_active_times = std::time(NULL);
        clients[clnt_sock].request += buf;
        size_t len = clients[clnt_sock].request.length();
        std::string tmp = clients[clnt_sock].request;
        if (len > 3 && !clients[clnt_sock].request.substr(len - 3).compare("\r\n\r")) {
            writeResponse(clnt_sock);
        }
    }
    delete[] buf;
}

void	Http::writeResponse(int clnt_sock) {
	std::pair<std::string, std::string>	response;

	std::cout << "clnt sock : " << clnt_sock << std::endl;
	std::cout << "req_msg : " << clients[clnt_sock].request << std::endl;
	if (clients.end() != clients.find(clnt_sock)) {
		if (!clients[clnt_sock].request.empty()) {
			err = 0;
		response.first = buildErrorMsg(clnt_sock);
		response.second = buildErrorHtml(clnt_sock);
		if ((write(clnt_sock, response.first.c_str(), response.first.length())) == -1)
			std::cerr << "Error : write error (response msg)" << std::endl;
		else
			std::cout << "write Successe (response msg)" << std::endl;
		if ((write(clnt_sock, response.second.c_str(), response.second.length())) == -1)
			std::cerr << "Error : write error (response content)" << std::endl;
		else
			std::cout << "write Successe (response content)" << std::endl;
		disconnectClient(clnt_sock);
		}
	}
}

void	Http::clientHandler() {
	for (int i = 0; i <= fd_max; i++) {
		if (FD_ISSET(i, &err_event))
			eventErrHandler(i);
		else if (FD_ISSET(i, &read_event))
			eventReadHandler(i);
	}
}

/*void	Http::clientHandler() {

	for (int i = 0; i < nevents; i++) {
		curr_event = &evlist[i];
		std::cout << "filter : " << curr_event->filter << std::endl;
		if (curr_event->flags & EV_ERROR)
			eventErrHandler(curr_event->ident);
		else if (curr_event->filter == EVFILT_READ)
			eventReadHandler(curr_event->ident);
		else if (curr_event->filter == EVFILT_WRITE)
			writeResponse(curr_event->ident);
	}
}*/