#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

void	err(const std::string msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

int main(int ac, char *av[])
{
	int	sock;
	std::string message;
	struct sockaddr_in serv_adr;
	char	*msg;

	if (ac != 3)
	{
		std::cerr << "Usage : " << av[0] << " <IP> <PORT>" << std::endl;
		exit(1);
	}
	// 소켓 생성
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		err("socket() error");
	// memset 반드시 해줘야함 구조체 내 쓰이지 않는 변수들 때문
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;					// protocol 버전
	serv_adr.sin_addr.s_addr = inet_addr(av[1]);	// 인터넷 주소
	serv_adr.sin_port = htons(atoi(av[2]));			// 프로토콜
	// 서버와 연결
	if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)))
		err("connect() error");
	while (1)
	{
		std::getline(std::cin, message);
		std::cout << message << std::endl;
		if (!message.compare("q"))
			break ;
		msg = (char *)message.c_str();
		write(sock, msg, strlen(msg));
	}
	close(sock);
	return 0;
}
