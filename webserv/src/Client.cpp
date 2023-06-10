#include "../header/Http.hpp"

void    Http::clientInit(uint16_t port, int clnt_sock) {
    clients[clnt_sock].port = ft_ntohs(port);
    clients[clnt_sock].request = "";
    clients[clnt_sock].last_active_times = std::time(NULL);
    clients[clnt_sock].str_len = 0;
	std::cout << "client connected : " << clnt_sock << " [" << clients[clnt_sock].port << "]" << std::endl;
}

void	Http::eventErrHandler(int serv_sock, int clnt_sock) {
	if (clnt_sock == serv_sock) {
		serverFunctionExecuteFailed(99, "socket", "server socket error");
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
	std::cout << "client disconnected : " << clnt_sock << " [" << clients[clnt_sock].port << "]"<< std::endl;
	close(clnt_sock);
	clients.erase(clnt_sock);
}

void	Http::clientAccept(int serv_sock, int clnt_sock, ServerBlock &server) {
	struct sockaddr_in	clnt_adr;
	socklen_t			clnt_adr_size;
	int					clnt_sock;

	clnt_adr_size = sizeof(clnt_adr);
	if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size)) == -1) {
        occurException(39, "accept()", HTTP, CLIENT,
        "accept function error");
    }		
    clientInit(clnt_adr.sin_port, clnt_sock);
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
	EV_SET(&server.chagelist, clnt_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	EV_SET(&server.chagelist, clnt_sock, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void	Http::readRequestMsg(int clnt_sock) {



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

void	Http::writeResponse(int clnt_sock) {

}

    std::string     request;
    std::time_t     last_active_times;
    unsigned short  port;
    std::ssize_t    str_len;