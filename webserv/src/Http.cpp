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
		if (fcntl(server.serv_sock, F_SETFL, O_NONBLOCK) == -1)
			serverFunctionExecuteFailed(36, "fcntl()");
		EV_SET(&server.chagelist, server.serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(this->kq, &server.chagelist, 1, NULL, 0, NULL) == -1)
			serverFunctionExecuteFailed(39, "kevent()");
	}
}
/*
void    Http::runServer()
{
	while (!flag) {
		if ((nevents = kevent(this->kq, NULL, 0, evlist, MAX_EVENTS, NULL)) == -1)
			serverFunctionExecuteFailed(47, "kevent()");

		signal(SIGINT, handle_signal);
		clientHandler();
	}
	
	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++)
		close(it->serv_sock);
}*/