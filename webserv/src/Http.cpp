#include "../header/Http.hpp"

volatile sig_atomic_t flag = 0;
void	handle_signal(int signum) { signum = 1;}
Http::Http() {}
Http::Http(const std::string &path)
{
	ParsingConfig(path);
	// printConfigInfo();
	checkValidConfig();
	SettingHttp();
	runServer();
}
Http::~Http() {}


void    Http::SettingHttp() {
	int t = 1;
	bool		serverOverlap;

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		serverOverlap = false;
		for (std::vector<ServerBlock>::iterator itt = this->server.begin(); itt != it; itt++) {
			if (it->port == itt->port) {
				serverOverlap = true;
				break ;
			}
		}
		if (!serverOverlap) {
			if ((it->serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				serverFunctionExecuteFailed(24, "socket())");
			std::memset(&it->serv_adr, 0, sizeof(it->serv_adr));
			it->serv_adr.sin_family = AF_INET;
			it->serv_adr.sin_addr.s_addr = inet_addr(it->host.c_str());
			it->serv_adr.sin_port = htons(it->port);
			if ((setsockopt(it->serv_sock, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t))) == -1)
				serverFunctionExecuteFailed(24, "setsockopt()");
			if ((bind(it->serv_sock, (struct sockaddr *)&it->serv_adr, sizeof(it->serv_adr))) == -1)
				serverFunctionExecuteFailed(24, "bind()");
		}
	}
}

void	Http::initializeServer() {
	bool		serverOverlap;

	FD_ZERO(&events);
	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++) {
		serverOverlap = false;
		for (std::vector<ServerBlock>::iterator itt = this->server.begin(); itt != it; itt++) {
			if (it->port == itt->port) {
				serverOverlap = true;
				break ;
			}
		}
		if (!serverOverlap) {
			if ((listen(it->serv_sock, LISTEN_SIZE)) == -1)
				serverFunctionExecuteFailed(47, "listen()");
			FD_SET(it->serv_sock, &events);
			if (fcntl(it->serv_sock, F_SETFL, O_NONBLOCK) == -1)
				serverFunctionExecuteFailed(47, "fcntl()");
			fd_max = it->serv_sock;
		}
	}
}

void    Http::runServer()
{
	initializeServer();
	while (!flag) {
		read_event = events;
		write_event = events;
		err_event = events;
		if ((select(fd_max + 1, &read_event, 0, &err_event, NULL)) < 0)
			serverFunctionExecuteFailed(136, "select()");
		clientHandler();
	}

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++)
		close(it->serv_sock);
}