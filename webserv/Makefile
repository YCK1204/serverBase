NAME = webserv
CPP = c++
SRCS = $(wildcard ./src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
FLAG = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

# %.o : %.c
# 	$(CPP) $(FLAGS) -c

$(NAME) : $(OBJS)
	$(CPP) $(FLAG) $(OBJS) -o $@

all : $(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: re all clean fclean
