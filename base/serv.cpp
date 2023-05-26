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

	// 소켓 생성
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		err("socket() error");
	// memset 반드시 해줘야함 구조체 내 쓰이지 않는 변수들 때문
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;						// procotol 버전
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);		// 인터넷 주소
	serv_addr.sin_port = htons(atoi(av[1]));			// 프로토콜

	// 소켓 할당
	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
		err("bind() error");
	// 연결 요청 대기열 생성
	if (listen(serv_sock, 5))
		err("listen() error");
	while (1)
	{
		// 연결 요청 수락
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		if (clnt_sock == -1)
			break ;
		while (1)
		{
			// 클라이언트로 부터 받아온 문자열 출력
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
