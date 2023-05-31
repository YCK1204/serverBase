#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int	main(void)
{
	fd_set	reads, tmp;
	int	result, str_len;
	char	buf[BUF_SIZE];
	struct timeval	timeout;

	FD_ZERO(&reads);
	FD_SET(0, &reads);
	while (1)
	{
		tmp = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(1, &tmp, 0, 0, &timeout);
		if (result == -1)
		{
			printf("select() error!\n");
			break ;
		}
		else if (!result)
			printf("Time-Out!\n");
		else if (FD_ISSET(0, &tmp))
		{
			str_len = read(0, buf, BUF_SIZE);
			buf[str_len] = 0;
			if (!std::strcmp(buf, "q\n"))
				break ;
			printf("%s\n", buf);
		}
	}
}
