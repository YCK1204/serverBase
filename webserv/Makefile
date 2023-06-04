NAME = webserv
CPP = c++
FLAG = -Wall -Wextra -Werror -std=c++98
SRCS = $(wildcard ./src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

%.o : %.c
	$(CPP) $(FLAG) -o $@

$(NAME) : $(OBJS)
	$(CPP) $(FLAG) -o $@ $^

all : $(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: re all clean fclean
