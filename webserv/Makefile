NAME = webserv
CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
SRCS = $(wildcard ./src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

%.o : %.c
	$(CPP) $(FLAGS) -c $^ -o $@

$(NAME) : $(OBJS)
	$(CPP) $(FLAGS) -o $@ $^

all : $(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: re all clean fclean
