#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

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
	struct timeval			timeout;
	fd_set					reads, cpy_reads;
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
	
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while (1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 1;
		timeout.tv_usec = 5000;

		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break ;
		else if (!fd_num)
		{
			std::cout << "Time-Out" << std::endl;
			continue ;
		}
		for (i = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &cpy_reads))
			{
				if (i == serv_sock)
				{
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
					std::cout << "connected clinet : " << clnt_sock << std::endl;
				}
				else
				{
					str_len = recv(i, buf, BUF_SIZE, 0);
					buf[str_len] = 0;
					std::cout << "server : " << buf << std::endl;
					if (!str_len)
					{
						FD_CLR(i, &reads);
						close(i);
						std::cout << "closed client : " << i << std::endl;
					}
					else
						write(i, buf, (size_t)str_len);
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}
