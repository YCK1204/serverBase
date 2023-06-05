#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUF_SIZE 1024

void	err(const std::string &msg)
{
	const char	*errMsg = msg.c_str();

	write(2, errMsg, strlen(errMsg));
	exit(1);
}

int	main(int ac, char *av[])
{
	int						serv_sock, clnt_sock, fd_max, str_len, fd_num, i;
	char					buf[BUF_SIZE];
	struct sockaddr_in		serv_adr, clnt_adr;
	struct timespec			timeout;
	socklen_t				adr_sz;
     

	if (ac != 2)
	{
		write(2, "Usage : ", 8);
		write(2, av[0], strlen(av[0]));
		write(2, " <port>\n", 8);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		err("socket() error!\n");
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(av[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
		err("bind() error!\n");
	if (listen(serv_sock, 2))
		err("listen() error!\n");
    
	fcntl(serv_sock, F_SETFL, O_NONBLOCK);

    /* init kqueue */
    int kq;
    if ((kq = kqueue()) == -1)
        err("kqueue() error\n");

    struct kevent change_event[4];
    EV_SET(change_event, serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
        err("kevent() error\n");
    
    int new_events;
    struct kevent event[4];
	while (1)
	{
		timeout.tv_sec = 1;
		timeout.tv_nsec = 5000;
        new_events = kevent(kq, NULL, 0, event, 8, (const timespec*)&timeout);
        if (new_events == 0)
		{
			std::cout << "Time-Out" << std::endl;
			continue ;
		}
		for (i = 0; i < new_events; i++)
		{
            int event_fd = event[i].ident;

            if (event[i].flags & EV_EOF)
            {
                std::cout << "closed client : " << event_fd << std::endl;
                close(event_fd);
            }
            // If the new event's file descriptor is the same as the listening
            // socket's file descriptor, we are sure that a new client wants 
            // to connect to our socket.
            else if (event_fd == serv_sock)
            {
                // Incoming socket connection on the listening socket.
                // Create a new socket for the actual connection to client.
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(event_fd, (struct sockaddr *)&clnt_adr, (socklen_t *)&adr_sz);
                if (clnt_sock == -1)
                {
                    perror("Accept socket error");
                }
                std::cout << "connected : " << clnt_sock << std::endl;

                // Put this new socket connection also as a 'filter' event
                // to watch in kqueue, so we can now watch for events on this
                // new socket.
                EV_SET(change_event, clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
                {
                    perror("kevent error");
                }
                
            }

            else if (event[i].filter & EVFILT_READ)
            {
                // Read bytes from socket
                size_t bytes_read = recv(event_fd, buf, BUF_SIZE, 0);
                printf("read %zu bytes\n", bytes_read);
                buf[bytes_read] = '3';
                buf[bytes_read + 1] = 0;
                printf("%s\n", buf);
                send(clnt_sock, buf, strlen(buf), 0);
            }
			
		}
	}
	close(serv_sock);
	return 0;
}
