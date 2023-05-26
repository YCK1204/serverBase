#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

void	err(const std::string msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

int	main(int ac, char *av[])
{
	int	serv_sock;
	int	clnt_sock;
	size_t a;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	char	message[1024];

	if (ac != 2)
	{
		printf("Usage : %s <port>n", av[0]);
		return (1);
	}
	
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		err("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(av[1]));

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
		err("bind() error");
	if (listen(serv_sock, 5))
		err("listen() error");
	while (1)
	{
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		if (clnt_sock == -1)
			break ;
		while (1)
		{
			a = read(clnt_sock, message, 1024);
			if (a <= 0)
				break ;
			message[a] = '\0';
			if (!strcmp(message, "q\n"))
				break ;
			std::cout << "server : " << message << std::endl;
		}
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
}
