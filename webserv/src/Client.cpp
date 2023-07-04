#include "../header/Http.hpp"

void    Http::clientInit(int clnt_sock) {
    clients[clnt_sock].str_len				= 0;
	clients[clnt_sock].root					= "";
	clients[clnt_sock].method				= "";
    clients[clnt_sock].request				= "";
	clients[clnt_sock].http_ver				= "";
	clients[clnt_sock].body_request			= "";
	clients[clnt_sock].file_extension		= "";
	clients[clnt_sock].connection			= "keep-alive";
    clients[clnt_sock].last_active_times	= std::time(NULL);
}

void	Http::eventErrHandler(int clnt_sock) {
	ServerBlock			server;

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		server = *it;
		if (clnt_sock == server.serv_sock) {
			occurException(12, "socket", HTTP, CLIENT,
			"server socket error");
		}
	}
	std::cerr << "client socket error" << std::endl;
	disconnectClient(clnt_sock);
}

void	Http::eventReadHandler(int clnt_sock) {
	bool				t = false;
	ServerBlock			server;

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		server = *it;
		if (clnt_sock == server.serv_sock) {
			clientAccept(server.serv_sock);
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
	std::cout << "client disconnected : " << clnt_sock << std::endl;
	clients.erase(clnt_sock);
	clearFdSet(clnt_sock, events);
}

void	Http::clientAccept(int serv_sock) {
	int					clnt_sk;
	struct sockaddr_in	clnt_adr;
	socklen_t			clnt_adr_size;

	clnt_adr_size = sizeof(clnt_adr);
	if ((clnt_sk = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1) {
        occurException(44, "accept()", HTTP, CLIENT,
        "accept function error");
    }
    clientInit(clnt_sk);
	clients[clnt_sk].request_cnt = 0;
	addFdSet(clnt_sk, events);
	fcntl(clnt_sk, F_SETFL, O_NONBLOCK);
	std::cout << "client connected : " << clnt_sk << std::endl;
}

void Http::readRequestMsg(int clnt_sock) {
    ssize_t n;
	char buf[BUF_SIZE + 1];

    n = read(clnt_sock, buf, BUF_SIZE);
    if (n <= 0) {
        disconnectClient(clnt_sock);
    } else if (n) {
        buf[n] = '\0';
        clients[clnt_sock].request += static_cast<std::string>(buf);
        clients[clnt_sock].str_len += n;
        clients[clnt_sock].last_active_times = std::time(NULL);
		std::size_t len = clients[clnt_sock].request.find("\r\n\r\n");
		if (len != std::string::npos) {
			if (clients[clnt_sock].request.find("POST") != std::string::npos) {
				std::size_t length = clients[clnt_sock].request.find("Content-Length:");
				if (length == std::string::npos) {
					err = 400;
					writeResponse(clnt_sock);
				} else {
					clients[clnt_sock].body_request = clients[clnt_sock].request.substr(len + 4);
					std::cout << clients[clnt_sock].body_request << std::endl;
					if (clients[clnt_sock].body_request.length() == static_cast<unsigned long>(ft_stoi(clients[clnt_sock].request.substr(length + 16))))
						writeResponse(clnt_sock);
				}
			}
			else
				writeResponse(clnt_sock);
		}
    }
}

void	Http::writeResponse(int clnt_sock) {
	std::pair<std::string, std::string>	response;
	ssize_t n;

	err = 0;
	status = 200;
	if (clients.end() != clients.find(clnt_sock)) {
		if (!clients[clnt_sock].request.empty()) {
			response = getResponse(clnt_sock);
			if ((n = (write(clnt_sock, response.first.c_str(), response.first.length()))) == -1)
				printLog(ORANGE, "Error : write error (response msg)", STDERR);
			if ((n = (write(clnt_sock, response.second.c_str(), response.second.length()))) == -1)
				printLog(ORANGE, "Error : write error (response content)", STDERR);
			if (n != -1) {
				std::string msg = "Response to client : " + ft_to_string(clnt_sock) + ", status=[";
				if (err != 0)
					printLog(RED, msg + ft_to_string(err) + "], method=[" + clients[clnt_sock].method + "], URI=" + clients[clnt_sock].root, STDOUT);
				else {
					msg += ft_to_string(status) + "], method=[" + clients[clnt_sock].method + "], URI=" + clients[clnt_sock].root;
					if (status >= 300)
						printLog(LIME, msg, STDOUT);
					else if (status >= 200)
						printLog(SKY_BLUE, msg, STDOUT);
					else
						printLog(YELLOW, msg, STDOUT);
				}
			}
			if (++clients[clnt_sock].request_cnt >= REQUEST_CNT)
				disconnectClient(clnt_sock);
			else {
				if (!clients[clnt_sock].connection.compare("close"))
					disconnectClient(clnt_sock);
				else
					clientInit(clnt_sock);
			}
		}
	}
}

void	Http::clientHandler() {
	for (int i = 0; i <= fd_max; i++) {
		if (FD_ISSET(i, &err_event))
			eventErrHandler(i);
		else if (FD_ISSET(i, &read_event)) {
			eventReadHandler(i);
		}
	}
	for (std::map<int, ClientData>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (!it->second.connection.compare("keep-alive") && (std::time(NULL) - it->second.last_active_times) > TIMEOUT) {
			disconnectClient(it->first);
			break ;
		}
	}
}