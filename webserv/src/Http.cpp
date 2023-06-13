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
	runServer();
}
Http::~Http() {}

void	Http::change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

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
			it->serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
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
			if ((listen(it->serv_sock, 20)) == -1)
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
		err_event = events;
		if ((select(fd_max + 1, &read_event, 0, &err_event, NULL)) < 0)
			serverFunctionExecuteFailed(136, "select()");
		clientHandler();
	}

	for (std::vector<ServerBlock>::iterator it = this->server.begin(); it != this->server.end(); it++)
		close(it->serv_sock);
}

/*void    Http::SettingHttp()
{
	int	reuse = 1;
	if ((this->kq = kqueue()) == -1)
		serverFunctionExecuteFailed(19, "kqueue()");
	std::cout << "fq file descriptor : " << this->kq << std::endl;
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
		change_events(changeList, server.serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		std::cout << "server file descriptor : " << server.serv_sock << std::endl;
	}
}*/


		// if ((nevents = kevent(this->kq, &changeList[0], changeList.size(), evlist, MAX_EVENTS, NULL)) == -1)
		// 	serverFunctionExecuteFailed(47, "kevent()");
		// changeList.clear();
		// signal(SIGINT, handle_signal);
